// Sem pch.h: este arquivo nao usa nada do Windows (ver SecureChannel.h).
#include "SecureChannel.h"

#include <string.h>

#include "../../Dependencies/monocypher/monocypher.h"

static const char SECURE_LABEL[] = "WYD-transport-v1";
constexpr int SECURE_LABEL_SIZE = sizeof(SECURE_LABEL) - 1;
constexpr int SECURE_NONCE_SIZE = 12;

static void SecureNonce(uint64_t count, uint8_t nonce[SECURE_NONCE_SIZE])
{
	memset(nonce, 0, SECURE_NONCE_SIZE);
	for (int i = 0; i < 8; ++i)
		nonce[i] = (uint8_t)(count >> (8 * i));
}

SecureChannel::SecureChannel()
{
	Reset();
}

SecureChannel::~SecureChannel()
{
	Reset();
}

void SecureChannel::Reset()
{
	crypto_wipe(m_Secret, sizeof(m_Secret));
	crypto_wipe(m_Public, sizeof(m_Public));
	crypto_wipe(m_SendKey, sizeof(m_SendKey));
	crypto_wipe(m_RecvKey, sizeof(m_RecvKey));
	m_nSendCount = 0;
	m_nRecvCount = 0;
	m_bReady = false;
}

void SecureChannel::Begin(const uint8_t random[SECURE_KEY_SIZE], uint8_t hello[SECURE_HELLO_SIZE])
{
	Reset();
	memcpy(m_Secret, random, SECURE_KEY_SIZE);
	crypto_x25519_public_key(m_Public, m_Secret);
	memcpy(hello, m_Public, SECURE_KEY_SIZE);
}

bool SecureChannel::Finish(const uint8_t serverKey[SECURE_KEY_SIZE], const uint8_t reply[SECURE_REPLY_SIZE])
{
	const uint8_t* serverEph = reply;
	const uint8_t* confirm = reply + SECURE_KEY_SIZE;

	// shared = efemera x efemera | efemera x chave fixa do servidor
	uint8_t shared[2 * SECURE_KEY_SIZE];
	crypto_x25519(shared, m_Secret, serverEph);
	crypto_x25519(shared + SECURE_KEY_SIZE, m_Secret, serverKey);

	// Um ponto de ordem baixa da um segredo todo zero, igual para qualquer um.
	static const uint8_t zero[SECURE_KEY_SIZE] = { 0 };
	bool weak = crypto_verify32(shared, zero) == 0 || crypto_verify32(shared + SECURE_KEY_SIZE, zero) == 0;

	uint8_t transcript[SECURE_LABEL_SIZE + 3 * SECURE_KEY_SIZE];
	memcpy(transcript, SECURE_LABEL, SECURE_LABEL_SIZE);
	memcpy(transcript + SECURE_LABEL_SIZE, m_Public, SECURE_KEY_SIZE);
	memcpy(transcript + SECURE_LABEL_SIZE + SECURE_KEY_SIZE, serverEph, SECURE_KEY_SIZE);
	memcpy(transcript + SECURE_LABEL_SIZE + 2 * SECURE_KEY_SIZE, serverKey, SECURE_KEY_SIZE);

	uint8_t keys[2 * SECURE_KEY_SIZE];
	crypto_blake2b_keyed(keys, sizeof(keys), shared, sizeof(shared), transcript, sizeof(transcript));
	memcpy(m_SendKey, keys, SECURE_KEY_SIZE);
	memcpy(m_RecvKey, keys + SECURE_KEY_SIZE, SECURE_KEY_SIZE);
	crypto_wipe(shared, sizeof(shared));
	crypto_wipe(keys, sizeof(keys));
	crypto_wipe(m_Secret, sizeof(m_Secret));

	// A confirmacao e a tag de uma mensagem vazia com o nonce 0, na chave
	// do servidor para o cliente.
	uint8_t nonce[SECURE_NONCE_SIZE];
	SecureNonce(0, nonce);
	crypto_aead_ctx ctx;
	crypto_aead_init_ietf(&ctx, m_RecvKey, nonce);
	int bad = crypto_aead_read(&ctx, nullptr, confirm, (const uint8_t*)SECURE_LABEL, SECURE_LABEL_SIZE, nullptr, 0);
	crypto_wipe(&ctx, sizeof(ctx));

	if (weak || bad != 0)
	{
		Reset();
		return false;
	}
	m_bReady = true;
	return true;
}

void SecureChannel::Seal(const uint8_t* packet, int size, uint8_t* frame)
{
	int frameSize = size + SECURE_TAG_SIZE;
	frame[0] = (uint8_t)frameSize;
	frame[1] = (uint8_t)(frameSize >> 8);

	uint8_t nonce[SECURE_NONCE_SIZE];
	SecureNonce(++m_nSendCount, nonce);
	crypto_aead_ctx ctx;
	crypto_aead_init_ietf(&ctx, m_SendKey, nonce);
	crypto_aead_write(&ctx, frame + 2, frame + size, frame, 2, packet + 2, size - 2);
	crypto_wipe(&ctx, sizeof(ctx));
}

int SecureChannel::Open(uint8_t* frame, int frameSize)
{
	int size = frameSize - SECURE_TAG_SIZE;
	if (!m_bReady || size < 2)
		return -1;

	uint8_t nonce[SECURE_NONCE_SIZE];
	SecureNonce(++m_nRecvCount, nonce);
	crypto_aead_ctx ctx;
	crypto_aead_init_ietf(&ctx, m_RecvKey, nonce);
	int bad = crypto_aead_read(&ctx, frame + 2, frame + size, frame, 2, frame + 2, size - 2);
	crypto_wipe(&ctx, sizeof(ctx));
	if (bad != 0)
		return -1;

	frame[0] = (uint8_t)size;
	frame[1] = (uint8_t)(size >> 8);
	return size;
}

static int SecureHexDigit(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

bool SecureParseKey(const char* text, uint8_t key[SECURE_KEY_SIZE])
{
	for (int i = 0; i < SECURE_KEY_SIZE; ++i)
	{
		int hi = SecureHexDigit(text[2 * i]);
		int lo = hi < 0 ? -1 : SecureHexDigit(text[2 * i + 1]);
		if (lo < 0)
			return false;
		key[i] = (uint8_t)(hi * 16 + lo);
	}
	for (const char* rest = text + 2 * SECURE_KEY_SIZE; *rest; ++rest)
	{
		if (*rest != ' ' && *rest != '\t' && *rest != '\r' && *rest != '\n')
			return false;
	}
	return true;
}
