package main

import (
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"sync"
	"syscall"
	"unsafe"
)

var (
	ole32 = syscall.NewLazyDLL("ole32.dll")
	atl   = syscall.NewLazyDLL("atl.dll")
	ntdll = syscall.NewLazyDLL("ntdll.dll")

	procOleInitialize          = ole32.NewProc("OleInitialize")
	procAtlAxWinInit           = atl.NewProc("AtlAxWinInit")
	procWineGetVersion         = ntdll.NewProc("wine_get_version")
	procPostMessage            = user32.NewProc("PostMessageW")
	procInvalidateRect         = user32.NewProc("InvalidateRect")
	procBeginPaint             = user32.NewProc("BeginPaint")
	procEndPaint               = user32.NewProc("EndPaint")
	procDrawText               = user32.NewProc("DrawTextW")
	procGetWindowText          = user32.NewProc("GetWindowTextW")
	procLoadImage              = user32.NewProc("LoadImageW")
	procCreateCompatibleDC     = gdi32.NewProc("CreateCompatibleDC")
	procCreateCompatibleBitmap = gdi32.NewProc("CreateCompatibleBitmap")
	procSelectObject           = gdi32.NewProc("SelectObject")
	procDeleteObject           = gdi32.NewProc("DeleteObject")
	procDeleteDC               = gdi32.NewProc("DeleteDC")
	procBitBlt                 = gdi32.NewProc("BitBlt")
	procExtTextOut             = gdi32.NewProc("ExtTextOutW")
	procSetBkColor             = gdi32.NewProc("SetBkColor")
	procSetBkMode              = gdi32.NewProc("SetBkMode")
	procSetTextColor           = gdi32.NewProc("SetTextColor")
	procCreateFont             = gdi32.NewProc("CreateFontW")
)

const (
	launcherClass = "WYDLauncherWindow"
	launcherTitle = "WYD"
	bannerFile    = "Launcher.bmp"

	launcherWidth  = 614
	launcherHeight = 470
	bannerX        = 8
	bannerY        = 20
	bannerWidth    = 598
	bannerHeight   = 369
	footerY        = bannerY + bannerHeight
	barX           = 136
	barWidth       = 300
	barHeight      = 14
	fileBarY       = 404
	totalBarY      = 424
	barCell        = 3 // 2 px aceso, 1 px de vão.
	wineFrame      = 2 // Moldura que o Wine desenha em volta do navegador.

	wsPopup        = 0x80000000
	wsSysMenu      = 0x00080000
	wsMinimizeBox  = 0x00020000
	wsClipChildren = 0x02000000
	wsDisabled     = 0x08000000
	bsOwnerDraw    = 0x0B

	wmPaint      = 0x000F
	wmEraseBkgnd = 0x0014
	wmDrawItem   = 0x002B
	wmNcHitTest  = 0x0084
	wmApp        = 0x8000
	wmProgress   = wmApp + 1
	wmShowPage   = wmApp + 2
	htClient     = 1
	htCaption    = 2

	odsSelected = 0x0001
	odsDisabled = 0x0004
	odsFocus    = 0x0010

	srcCopy         = 0x00CC0020
	etoOpaque       = 2
	bkTransparent   = 1
	dtCenter        = 0x01
	dtVCenter       = 0x04
	dtSingleLine    = 0x20
	dtEndEllipsis   = 0x8000
	imageBitmap     = 0
	lrLoadFromFile  = 0x10
	fwNormal        = 400
	fwBold          = 700
	defaultCharset  = 1
	antialiasedFont = 4

	colorWindow  = 0x0D0A08
	colorLine    = 0x524638
	colorBanner  = 0x000000
	colorBarBack = 0x050403
	colorBarCell = 0xD8D2CC
	colorFace    = 0x3E3630
	colorFaceTop = 0x564C44
	colorFaceHot = 0x2C2622
	colorFaceOff = 0x1E1A17
	colorText    = 0xEEEAE6
	colorTextOff = 0x70665E
	colorStatus  = 0xA89E94
	colorFocus   = 0xC8BEB2
)

const idOptions, idExit = 3, 4

type paintStruct struct {
	hdc        uintptr
	erase      int32
	paint      rect
	restore    int32
	incUpdate  int32
	rgbReserve [32]byte
}

type drawItem struct {
	ctlType, ctlID, itemID, action, state uint32
	hwnd, hdc                             uintptr
	item                                  rect
	data                                  uintptr
}

type launcherWindow struct {
	dir, cfgPath string
	cfg          *config
	instance     uintptr
	hwnd         uintptr
	font         uintptr
	bigFont      uintptr
	banner       uintptr
	browser      uintptr
	start        uintptr
	begin        bool

	mu       sync.Mutex
	progress Progress
	pageURL  string
}

func runningOnWine() bool {
	return procWineGetVersion.Find() == nil
}

// browserAvailable: no Windows o controle é do sistema. No Wine depende do
// wine-gecko que o WYD-Linux instala no prefixo; sem ele a área ficaria branca.
func browserAvailable() bool {
	if !runningOnWine() {
		return true
	}
	engines, _ := filepath.Glob(filepath.Join(os.Getenv("SystemRoot"), "system32", "gecko", "*", "wine_gecko"))
	return len(engines) > 0
}

func newFont(height, weight int) uintptr {
	font, _, _ := procCreateFont.Call(uintptr(-height&0xFFFFFFFF), 0, 0, 0, uintptr(weight), 0, 0, 0,
		defaultCharset, 0, 0, antialiasedFont, 0, uintptr(unsafe.Pointer(text("Tahoma"))))
	return font
}

func fill(hdc uintptr, area rect, color uintptr) {
	procSetBkColor.Call(hdc, color)
	procExtTextOut.Call(hdc, 0, 0, etoOpaque, uintptr(unsafe.Pointer(&area)), 0, 0, 0)
}

func frame(hdc uintptr, area rect, color uintptr) {
	fill(hdc, rect{area.left, area.top, area.right, area.top + 1}, color)
	fill(hdc, rect{area.left, area.bottom - 1, area.right, area.bottom}, color)
	fill(hdc, rect{area.left, area.top, area.left + 1, area.bottom}, color)
	fill(hdc, rect{area.right - 1, area.top, area.right, area.bottom}, color)
}

func drawLabel(hdc uintptr, label string, area rect, color, font uintptr) {
	old, _, _ := procSelectObject.Call(hdc, font)
	procSetBkMode.Call(hdc, bkTransparent)
	procSetTextColor.Call(hdc, color)
	procDrawText.Call(hdc, uintptr(unsafe.Pointer(text(label))), ^uintptr(0),
		uintptr(unsafe.Pointer(&area)), dtCenter|dtVCenter|dtSingleLine|dtEndEllipsis)
	procSelectObject.Call(hdc, old)
}

// drawBar desenha a barra em células, como a do launcher original.
func drawBar(hdc uintptr, y int32, part float64) {
	outer := rect{barX, y, barX + barWidth, y + barHeight}
	fill(hdc, outer, colorBarBack)
	frame(hdc, outer, colorLine)
	inner := int32(barWidth - 4)
	cells := int32(part * float64(inner/barCell))
	for i := int32(0); i < cells; i++ {
		x := barX + 2 + i*barCell
		fill(hdc, rect{x, y + 2, x + barCell - 1, y + barHeight - 2}, colorBarCell)
	}
}

func (l *launcherWindow) drawBanner(hdc uintptr) {
	area := rect{bannerX, bannerY, bannerX + bannerWidth, bannerY + bannerHeight}
	fill(hdc, area, colorBanner)
	if l.banner == 0 {
		drawLabel(hdc, launcherTitle, area, colorStatus, l.bigFont)
		return
	}
	mem, _, _ := procCreateCompatibleDC.Call(hdc)
	old, _, _ := procSelectObject.Call(mem, l.banner)
	procBitBlt.Call(hdc, bannerX, bannerY, bannerWidth, bannerHeight, mem, 0, 0, srcCopy)
	procSelectObject.Call(mem, old)
	procDeleteDC.Call(mem)
}

func (l *launcherWindow) paint() {
	var ps paintStruct
	target, _, _ := procBeginPaint.Call(l.hwnd, uintptr(unsafe.Pointer(&ps)))
	defer procEndPaint.Call(l.hwnd, uintptr(unsafe.Pointer(&ps)))

	// Tudo vai para um bitmap e depois para a tela, para as barras não piscarem.
	hdc, _, _ := procCreateCompatibleDC.Call(target)
	defer procDeleteDC.Call(hdc)
	surface, _, _ := procCreateCompatibleBitmap.Call(target, launcherWidth, launcherHeight)
	defer procDeleteObject.Call(surface)
	old, _, _ := procSelectObject.Call(hdc, surface)
	defer procSelectObject.Call(hdc, old)

	l.mu.Lock()
	state := l.progress
	l.mu.Unlock()

	whole := rect{0, 0, launcherWidth, launcherHeight}
	fill(hdc, whole, colorWindow)
	frame(hdc, whole, colorLine)
	l.drawBanner(hdc)
	frame(hdc, rect{bannerX - 1, bannerY - 1, bannerX + bannerWidth + 1, bannerY + bannerHeight + 1}, colorLine)
	drawBar(hdc, fileBarY, state.File)
	drawBar(hdc, totalBarY, state.Total)
	drawLabel(hdc, state.Status, rect{bannerX, 449, bannerX + bannerWidth, launcherHeight - 2}, colorStatus, l.font)

	procBitBlt.Call(target, 0, 0, launcherWidth, launcherHeight, hdc, 0, 0, srcCopy)
}

func (l *launcherWindow) drawButton(item *drawItem) {
	var buffer [64]uint16
	procGetWindowText.Call(item.hwnd, uintptr(unsafe.Pointer(&buffer)), uintptr(len(buffer)))
	label := syscall.UTF16ToString(buffer[:])

	face, top, color := uintptr(colorFace), uintptr(colorFaceTop), uintptr(colorText)
	switch {
	case item.state&odsDisabled != 0:
		face, top, color = colorFaceOff, colorFaceOff, colorTextOff
	case item.state&odsSelected != 0:
		face, top = colorFaceHot, colorFaceHot
	}
	area := item.item
	fill(item.hdc, area, face)
	fill(item.hdc, rect{area.left, area.top, area.right, area.top + (area.bottom-area.top)/2}, top)
	border := uintptr(colorLine)
	if item.state&odsFocus != 0 {
		border = colorFocus
	}
	frame(item.hdc, area, border)
	font := l.font
	if item.ctlID == idOK {
		font = l.bigFont
	}
	drawLabel(item.hdc, label, area, color, font)
}

func (l *launcherWindow) addButton(title string, style uint32, x, y, width, height int, id uintptr) uintptr {
	handle, _, _ := procCreateWindowEx.Call(0,
		uintptr(unsafe.Pointer(text("BUTTON"))), uintptr(unsafe.Pointer(text(title))),
		uintptr(wsChild|wsVisible|bsOwnerDraw|style), uintptr(x), uintptr(y), uintptr(width), uintptr(height),
		l.hwnd, id, l.instance, 0)
	return handle
}

// showPage põe a página do servidor sobre a arte local. AtlAxWin hospeda o
// controle de navegador e navega para a URL que recebe como título.
func (l *launcherWindow) showPage() {
	l.mu.Lock()
	address := l.pageURL
	l.mu.Unlock()
	if l.browser != 0 || address == "" || atl.Load() != nil {
		return
	}
	if ok, _, _ := procAtlAxWinInit.Call(); ok == 0 {
		return
	}
	// O CSS da página não alcança a moldura do Wine: o controle fica maior que a
	// área e o recorte esconde a sobra.
	frame := 0
	if runningOnWine() {
		frame = wineFrame
	}
	clip, _, _ := procCreateWindowEx.Call(0,
		uintptr(unsafe.Pointer(text("STATIC"))), uintptr(unsafe.Pointer(text(""))),
		wsChild|wsVisible|wsClipChildren, bannerX, bannerY, bannerWidth, bannerHeight, l.hwnd, 0, l.instance, 0)
	l.browser, _, _ = procCreateWindowEx.Call(0,
		uintptr(unsafe.Pointer(text("AtlAxWin"))), uintptr(unsafe.Pointer(text(address))),
		wsChild|wsVisible, uintptr(-frame), uintptr(-frame), uintptr(bannerWidth+2*frame), uintptr(bannerHeight+2*frame),
		clip, 0, l.instance, 0)
}

func (l *launcherWindow) applyProgress() {
	l.mu.Lock()
	state := l.progress
	l.mu.Unlock()
	footer := rect{0, footerY, launcherWidth, launcherHeight}
	procInvalidateRect.Call(l.hwnd, uintptr(unsafe.Pointer(&footer)), 0)
	// Desligado enquanto uma atualização roda, inclusive a que a troca de
	// servidor nas Configurações dispara.
	procEnableWindow.Call(l.start, uintptr(boolValue(state.Done && state.Ready)))
}

func (l *launcherWindow) proc(hwnd, message, wParam, lParam uintptr) uintptr {
	switch message {
	case wmEraseBkgnd:
		return 1
	case wmPaint:
		l.paint()
		return 0
	case wmDrawItem:
		l.drawButton(*(**drawItem)(unsafe.Pointer(&lParam)))
		return 1
	case wmNcHitTest:
		// Sem barra de título: arrastar pelo fundo move a janela.
		result, _, _ := procDefWindowProc.Call(hwnd, message, wParam, lParam)
		if result == htClient {
			return htCaption
		}
		return result
	case wmProgress:
		l.applyProgress()
		return 0
	case wmShowPage:
		l.showPage()
		return 0
	case wmCommand:
		switch wParam & 0xFFFF {
		case idOK:
			l.mu.Lock()
			ready := l.progress.Done && l.progress.Ready
			l.mu.Unlock()
			if ready {
				l.begin = true
				procDestroyWindow.Call(hwnd)
			}
			return 0
		case idOptions:
			changed, err := runSettings(hwnd, l.cfg, l.cfgPath, l.dir)
			if err != nil {
				messageBox(err.Error())
			}
			if changed {
				l.setProgress(Progress{Status: "Verificando atualizações..."})
				go newUpdater(l.dir, l.setProgress).run()
			}
			return 0
		case idCancel, idExit:
			procDestroyWindow.Call(hwnd)
			return 0
		}
	case wmDestroy:
		procPostQuitMessage.Call(0)
		return 0
	}
	result, _, _ := procDefWindowProc.Call(hwnd, message, wParam, lParam)
	return result
}

func (l *launcherWindow) create() error {
	l.instance, _, _ = procGetModuleHandle.Call(0)
	l.font = newFont(12, fwNormal)
	l.bigFont = newFont(18, fwBold)
	l.banner, _, _ = procLoadImage.Call(0, uintptr(unsafe.Pointer(text(filepath.Join(l.dir, bannerFile)))),
		imageBitmap, 0, 0, lrLoadFromFile)

	icon, _, _ := procLoadIcon.Call(l.instance, mainIconID)
	cursor, _, _ := procLoadCursor.Call(0, idcArrow)
	class := wndClassEx{
		proc:      syscall.NewCallback(l.proc),
		instance:  l.instance,
		icon:      icon,
		iconSmall: icon,
		cursor:    cursor,
		className: text(launcherClass),
	}
	class.size = uint32(unsafe.Sizeof(class))
	if atom, _, err := procRegisterClassEx.Call(uintptr(unsafe.Pointer(&class))); atom == 0 {
		return fmt.Errorf("não foi possível criar a janela: %v", err)
	}

	screenWidth, _, _ := procGetSystemMetrics.Call(smCXScreen)
	screenHeight, _, _ := procGetSystemMetrics.Call(smCYScreen)
	var err error
	l.hwnd, _, err = procCreateWindowEx.Call(0,
		uintptr(unsafe.Pointer(class.className)), uintptr(unsafe.Pointer(text(launcherTitle))),
		wsPopup|wsSysMenu|wsMinimizeBox|wsClipChildren,
		(screenWidth-launcherWidth)/2, (screenHeight-launcherHeight)/2,
		launcherWidth, launcherHeight, 0, 0, l.instance, 0)
	if l.hwnd == 0 {
		return fmt.Errorf("não foi possível criar a janela: %v", err)
	}

	l.start = l.addButton("Iniciar", wsTabStop|wsDisabled, 14, 400, 110, 44, idOK)
	l.addButton("Configuração", wsTabStop, 446, 412, 90, 27, idOptions)
	l.addButton("Cancelar", wsTabStop, 542, 412, 62, 27, idCancel)
	l.addButton("×", 0, launcherWidth-22, 3, 16, 14, idExit)
	procShowWindow.Call(l.hwnd, swShow)
	return nil
}

func (l *launcherWindow) setProgress(p Progress) {
	l.mu.Lock()
	l.progress = p
	l.mu.Unlock()
	procPostMessage.Call(l.hwnd, wmProgress, 0, 0)
}

// loadPage confere a página fora da thread da janela.
func (l *launcherWindow) loadPage() {
	if !browserAvailable() {
		return
	}
	base, err := launcherBase(l.dir)
	if err != nil || !pageAvailable(base) {
		return
	}
	l.mu.Lock()
	l.pageURL = base + launcherPage
	l.mu.Unlock()
	procPostMessage.Call(l.hwnd, wmShowPage, 0, 0)
}

// runLauncher só volta sem erro depois do botão Iniciar.
func runLauncher(dir, cfgPath string, cfg *config) error {
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	procOleInitialize.Call(0)

	l := &launcherWindow{dir: dir, cfgPath: cfgPath, cfg: cfg, progress: Progress{Status: "Verificando atualizações..."}}
	if err := l.create(); err != nil {
		return err
	}
	go l.loadPage()
	go newUpdater(dir, l.setProgress).run()

	var m msg
	for {
		result, _, _ := procGetMessage.Call(uintptr(unsafe.Pointer(&m)), 0, 0, 0)
		if int32(result) <= 0 {
			break
		}
		if handled, _, _ := procIsDialogMessage.Call(l.hwnd, uintptr(unsafe.Pointer(&m))); handled != 0 {
			continue
		}
		procTranslateMessage.Call(uintptr(unsafe.Pointer(&m)))
		procDispatchMessage.Call(uintptr(unsafe.Pointer(&m)))
	}
	if !l.begin {
		return errClosed
	}
	return nil
}
