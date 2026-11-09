// Compilado apenas no build do Linux (CMakeLists.txt); nao faz parte do TMProject.vcxproj.
// O winedbg nem sempre consegue gerar o backtrace, entao o proprio jogo imprime a pilha.
// Para traduzir os enderecos: x86_64-w64-mingw32-addr2line -f -C -e build-linux/WYD.exe <enderecos>
#include <windows.h>
#include <cstdio>

static void WydPrintStack(CONTEXT context)
{
	// Chamada de ponteiro nulo: o endereco de retorno esta no topo da pilha.
	if (context.Rip == 0)
	{
		context.Rip = *reinterpret_cast<DWORD64*>(context.Rsp);
		context.Rsp += 8;
	}

	for (int nFrame = 0; nFrame < 32 && context.Rip != 0; ++nFrame)
	{
		printf("[wyd-crash] #%02d %016llX\n", nFrame, context.Rip);

		DWORD64 imageBase = 0;
		PRUNTIME_FUNCTION pFunction = RtlLookupFunctionEntry(context.Rip, &imageBase, nullptr);
		if (pFunction == nullptr)
		{
			context.Rip = *reinterpret_cast<DWORD64*>(context.Rsp);
			context.Rsp += 8;
			continue;
		}

		PVOID pHandlerData = nullptr;
		DWORD64 establisherFrame = 0;
		RtlVirtualUnwind(UNW_FLAG_NHANDLER, imageBase, context.Rip, pFunction,
			&context, &pHandlerData, &establisherFrame, nullptr);
	}

	fflush(stdout);
}

static LONG WINAPI WydCrashFilter(EXCEPTION_POINTERS* pInfo)
{
	printf("\n[wyd-crash] excecao %08lX em %p, base %p\n",
		pInfo->ExceptionRecord->ExceptionCode,
		pInfo->ExceptionRecord->ExceptionAddress,
		GetModuleHandleA(nullptr));
	WydPrintStack(*pInfo->ContextRecord);
	return EXCEPTION_CONTINUE_SEARCH;
}

static const LPTOP_LEVEL_EXCEPTION_FILTER g_pPreviousCrashFilter = SetUnhandledExceptionFilter(WydCrashFilter);
