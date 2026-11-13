#pragma once

#include <stdint.h>

// Transporte seguro entre o cliente e a TMSRV. Substitui a cifra de tabela
// fixa (pKeyWord), que qualquer pessoa com o cliente consegue desfazer.
//
//   cliente -> servidor   INIT_CODE (4) | chave efemera X25519 do cliente (32)
//   servidor -> cliente   chave efemera X25519 do servidor (32) | confirmacao (16)
//
// As duas chaves da conexao saem de BLAKE2b sobre os dois segredos X25519
// (efemera x efemera e efemera x chave fixa do servidor). So quem tem a chave
// privada do servidor chega nelas, e a confirmacao deixa o cliente recusar um
// servidor falso antes de enviar o login. A chave publica do servidor vem de
// serverkey.txt, na pasta do jogo.
//
// Depois disso cada pacote viaja em um quadro:
//
//   uint16 W (tamanho do quadro) | pacote cifrado a partir do byte 2 | tag (16)
//
// com ChaCha20-Poly1305 (RFC 8439) e um contador por sentido como nonce: um
// quadro alterado, repetido ou fora de ordem e recusado.
//
// O servidor implementa o mesmo em internal/protocol/secure.go, e os dois sao
// conferidos pelo mesmo vetor de teste (TestVectorsMatchTheClient).
// Este arquivo nao depende do Windows: e compilado tambem pelo teste.

constexpr int SECURE_KEY_SIZE = 32;
constexpr int SECURE_TAG_SIZE = 16;
constexpr int SECURE_HELLO_SIZE = SECURE_KEY_SIZE;
constexpr int SECURE_REPLY_SIZE = SECURE_KEY_SIZE + SECURE_TAG_SIZE;

class SecureChannel
{
public:
	SecureChannel();
	~SecureChannel();

	// Apaga as chaves; a conexao seguinte comeca de novo.
	void Reset();

	// Comeca a troca de chaves. random sao 32 bytes do gerador do sistema;
	// hello recebe o que vai para o servidor logo depois do INIT_CODE.
	void Begin(const uint8_t random[SECURE_KEY_SIZE], uint8_t hello[SECURE_HELLO_SIZE]);

	// Termina a troca com a resposta do servidor. Devolve false se quem
	// respondeu nao tem a chave privada de serverKey.
	bool Finish(const uint8_t serverKey[SECURE_KEY_SIZE], const uint8_t reply[SECURE_REPLY_SIZE]);

	bool Ready() const { return m_bReady; }

	// Monta em frame o quadro de um pacote ja com o cabecalho preenchido.
	// frame precisa de size + SECURE_TAG_SIZE bytes.
	void Seal(const uint8_t* packet, int size, uint8_t* frame);

	// Abre no lugar um quadro completo. Devolve o tamanho do pacote, que
	// fica no inicio de frame com o campo Size certo, ou -1 se o quadro nao
	// for autentico.
	int Open(uint8_t* frame, int frameSize);

private:
	uint8_t m_Secret[SECURE_KEY_SIZE];
	uint8_t m_Public[SECURE_KEY_SIZE];
	uint8_t m_SendKey[SECURE_KEY_SIZE];
	uint8_t m_RecvKey[SECURE_KEY_SIZE];
	uint64_t m_nSendCount;
	uint64_t m_nRecvCount;
	bool m_bReady;
};

// Le 64 digitos hexadecimais (espacos e quebra de linha no fim sao aceitos).
bool SecureParseKey(const char* text, uint8_t key[SECURE_KEY_SIZE]);
