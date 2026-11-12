package main

import (
	"errors"
	"fmt"
	"strings"
	"syscall"
	"unsafe"
)

var (
	user32   = syscall.NewLazyDLL("user32.dll")
	kernel32 = syscall.NewLazyDLL("kernel32.dll")
	gdi32    = syscall.NewLazyDLL("gdi32.dll")
	comctl32 = syscall.NewLazyDLL("comctl32.dll")

	procRegisterClassEx     = user32.NewProc("RegisterClassExW")
	procCreateWindowEx      = user32.NewProc("CreateWindowExW")
	procDefWindowProc       = user32.NewProc("DefWindowProcW")
	procDestroyWindow       = user32.NewProc("DestroyWindow")
	procShowWindow          = user32.NewProc("ShowWindow")
	procGetMessage          = user32.NewProc("GetMessageW")
	procIsDialogMessage     = user32.NewProc("IsDialogMessageW")
	procTranslateMessage    = user32.NewProc("TranslateMessage")
	procDispatchMessage     = user32.NewProc("DispatchMessageW")
	procPostQuitMessage     = user32.NewProc("PostQuitMessage")
	procSendMessage         = user32.NewProc("SendMessageW")
	procSetWindowText       = user32.NewProc("SetWindowTextW")
	procMessageBox          = user32.NewProc("MessageBoxW")
	procLoadIcon            = user32.NewProc("LoadIconW")
	procLoadCursor          = user32.NewProc("LoadCursorW")
	procAdjustWindowRect    = user32.NewProc("AdjustWindowRect")
	procGetSystemMetrics    = user32.NewProc("GetSystemMetrics")
	procEnumDisplaySettings = user32.NewProc("EnumDisplaySettingsW")
	procEnableWindow        = user32.NewProc("EnableWindow")
	procGetModuleHandle     = kernel32.NewProc("GetModuleHandleW")
	procGetStockObject      = gdi32.NewProc("GetStockObject")
	procInitCommonControls  = comctl32.NewProc("InitCommonControlsEx")
)

const (
	wsChild        = 0x40000000
	wsVisible      = 0x10000000
	wsTabStop      = 0x00010000
	wsVScroll      = 0x00200000
	wsBorder       = 0x00800000
	esAutoHScroll  = 0x80
	emLimitText    = 0x00C5
	wsMainWindow   = 0x00CA0000 | 0x00020000 // WS_CAPTION | WS_SYSMENU, com minimizar.
	wsExControl    = 0x00010000
	bsDefault      = 0x1
	bsCheck        = 0x3
	bsGroup        = 0x7
	cbsDropDown    = 0x3
	tbsNoTicks     = 0x10
	wmDestroy      = 0x0002
	wmClose        = 0x0010
	wmSetFont      = 0x0030
	wmCommand      = 0x0111
	wmHScroll      = 0x0114
	bmGetCheck     = 0x00F0
	bmSetCheck     = 0x00F1
	cbAddString    = 0x0143
	cbGetCurSel    = 0x0147
	cbSetCurSel    = 0x014E
	cbGetItemData  = 0x0150
	cbSetItemData  = 0x0151
	tbmGetPos      = 0x0400
	tbmSetPos      = 0x0405
	tbmSetRange    = 0x0406
	idOK           = 1 // Enter.
	idCancel       = 2 // Esc.
	colorBtnFace   = 15
	defaultGUIFont = 17
	iccBarClasses  = 0x4
	mbIconError    = 0x10
	smCXScreen     = 0
	smCYScreen     = 1
	swShow         = 5
	idcArrow       = 32512
	mainIconID     = 1
	windowClass    = "JogarWYDWindow"
	windowTitle    = "WYD - Configurações"
	errorTitle     = "WYD"
	clientWidth    = 400
	clientHeight   = 478
	labelX         = 26
	controlX       = 120
	controlWidth   = 250
	sliderHeight   = 26
	checkWidth     = 350
	checkHeight    = 20
	requiredColors = 32
)

var errClosed = errors.New("janela fechada")

type wndClassEx struct {
	size       uint32
	style      uint32
	proc       uintptr
	clsExtra   int32
	wndExtra   int32
	instance   uintptr
	icon       uintptr
	cursor     uintptr
	background uintptr
	menuName   *uint16
	className  *uint16
	iconSmall  uintptr
}

type msg struct {
	hwnd    uintptr
	message uint32
	wParam  uintptr
	lParam  uintptr
	time    uint32
	x, y    int32
	private uint32
}

type rect struct {
	left, top, right, bottom int32
}

type initCommonControls struct {
	size, classes uint32
}

// devMode é o DEVMODEW: só interessam dmSize e os campos de bits e tamanho.
type devMode struct {
	_      [68]byte
	size   uint16
	_      [98]byte
	bits   uint32
	width  uint32
	height uint32
	_      [40]byte
}

func text(s string) *uint16 {
	p, err := syscall.UTF16PtrFromString(s)
	if err != nil {
		p, _ = syscall.UTF16PtrFromString("")
	}
	return p
}

func messageBox(body string) {
	procMessageBox.Call(0, uintptr(unsafe.Pointer(text(body))), uintptr(unsafe.Pointer(text(errorTitle))), mbIconError)
}

// displaySupports faz a mesma busca de NewApp::CheckResolution. Modo que ela
// recusa faria o cliente abrir em 800x600 com o layout de outra resolução.
func displaySupports(width, height uint32) bool {
	var mode devMode
	mode.size = uint16(unsafe.Sizeof(mode))
	for i := uintptr(0); ; i++ {
		ok, _, _ := procEnumDisplaySettings.Call(0, i, uintptr(unsafe.Pointer(&mode)))
		if ok == 0 {
			return false
		}
		if mode.width == width && mode.height == height && mode.bits == requiredColors {
			return true
		}
	}
}

func availableResolutions() []Resolution {
	list := []Resolution{resolutions[0]}
	for _, r := range resolutions[1:] {
		if displaySupports(r.Width, r.Height) {
			list = append(list, r)
		}
	}
	return list
}

func selectableResolution(list []Resolution, id int) int {
	for _, r := range list {
		if r.ID == id {
			return id
		}
	}
	return list[0].ID
}

type slider struct {
	handle uintptr
	label  uintptr
	title  string
}

type window struct {
	cfg        *config
	path       string
	owner      uintptr
	instance   uintptr
	hwnd       uintptr
	font       uintptr
	done       bool
	resolution uintptr
	fullscreen uintptr
	cursor     uintptr
	camera     uintptr
	textures   uintptr
	chat       uintptr
	server     uintptr
	dir        string
	shownIP    string // Endereço que a caixa mostrou ao abrir.
	changed    bool   // O servidor mudou: o launcher precisa atualizar de novo.
	bright     slider
	sound      slider
	music      slider
}

func send(handle, message, wParam, lParam uintptr) uintptr {
	result, _, _ := procSendMessage.Call(handle, message, wParam, lParam)
	return result
}

func (w *window) add(class, title string, style uint32, x, y, width, height int, id uintptr) uintptr {
	handle, _, _ := procCreateWindowEx.Call(0,
		uintptr(unsafe.Pointer(text(class))), uintptr(unsafe.Pointer(text(title))),
		uintptr(wsChild|wsVisible|style), uintptr(x), uintptr(y), uintptr(width), uintptr(height),
		w.hwnd, id, w.instance, 0)
	send(handle, wmSetFont, w.font, 1)
	return handle
}

func (w *window) addSlider(title string, y, value int) slider {
	s := slider{title: title}
	s.label = w.add("STATIC", "", 0, labelX, y+4, controlX-labelX-4, 18, 0)
	s.handle = w.add("msctls_trackbar32", "", wsTabStop|tbsNoTicks, controlX, y, controlWidth, sliderHeight, 0)
	send(s.handle, tbmSetRange, 1, uintptr(maxPercent)<<16)
	send(s.handle, tbmSetPos, 1, uintptr(value))
	s.refresh()
	return s
}

func (s slider) value() int {
	return int(send(s.handle, tbmGetPos, 0, 0))
}

func (s slider) refresh() {
	procSetWindowText.Call(s.label, uintptr(unsafe.Pointer(text(fmt.Sprintf("%s: %d%%", s.title, s.value())))))
}

func (w *window) addCheck(title string, y int, checked bool) uintptr {
	handle := w.add("BUTTON", title, wsTabStop|bsCheck, labelX, y, checkWidth, checkHeight, 0)
	send(handle, bmSetCheck, uintptr(boolValue(checked)), 0)
	return handle
}

func (w *window) build() {
	s := w.cfg.settings()

	w.add("BUTTON", "Vídeo", bsGroup, 12, 8, clientWidth-24, 118, 0)
	w.add("STATIC", "Resolução:", 0, labelX, 33, 90, 18, 0)
	w.resolution = w.add("COMBOBOX", "", wsTabStop|wsVScroll|cbsDropDown, controlX, 29, controlWidth, 200, 0)
	list := availableResolutions()
	selected := selectableResolution(list, s.Resolution)
	for i, r := range list {
		send(w.resolution, cbAddString, 0, uintptr(unsafe.Pointer(text(fmt.Sprintf("%d x %d", r.Width, r.Height)))))
		send(w.resolution, cbSetItemData, uintptr(i), uintptr(r.ID))
		if r.ID == selected {
			send(w.resolution, cbSetCurSel, uintptr(i), 0)
		}
	}
	w.fullscreen = w.addCheck("Tela cheia", 60, s.Fullscreen)
	w.bright = w.addSlider("Brilho", 88, s.Bright)

	w.add("BUTTON", "Áudio", bsGroup, 12, 132, clientWidth-24, 84, 0)
	w.sound = w.addSlider("Efeitos", 154, s.Sound)
	w.music = w.addSlider("Música", 184, s.Music)

	w.add("BUTTON", "Jogo", bsGroup, 12, 222, clientWidth-24, 134, 0)
	w.cursor = w.addCheck("Cursor do sistema (responde mais rápido)", 244, s.SystemCursor)
	w.camera = w.addCheck("Inverter o giro da câmera", 270, s.CameraRotate)
	w.textures = w.addCheck("Texturas comprimidas (usa menos memória)", 296, s.CompressedTextures)
	w.chat = w.addCheck("Digitar no chat sem apertar Enter", 322, s.ChatAlwaysOn)

	w.add("BUTTON", "Servidor", bsGroup, 12, 362, clientWidth-24, 62, 0)
	w.add("STATIC", "Endereço IP:", 0, labelX, 388, 90, 18, 0)
	w.shownIP = currentServerAddress(w.dir)
	w.server = w.add("EDIT", w.shownIP, wsTabStop|wsBorder|esAutoHScroll, controlX, 384, controlWidth, 22, 0)
	send(w.server, emLimitText, maxAddressLength, 0)

	w.add("BUTTON", "Salvar", wsTabStop|bsDefault, clientWidth-204, 434, 90, 30, idOK)
	w.add("BUTTON", "Cancelar", wsTabStop, clientWidth-102, 434, 90, 30, idCancel)
}

func checked(handle uintptr) bool {
	return send(handle, bmGetCheck, 0, 0) == 1
}

func (w *window) collect() Settings {
	index := send(w.resolution, cbGetCurSel, 0, 0)
	return Settings{
		Resolution:         int(send(w.resolution, cbGetItemData, index, 0)),
		Fullscreen:         checked(w.fullscreen),
		Bright:             w.bright.value(),
		Sound:              w.sound.value(),
		Music:              w.music.value(),
		SystemCursor:       checked(w.cursor),
		CameraRotate:       checked(w.camera),
		CompressedTextures: checked(w.textures),
		ChatAlwaysOn:       checked(w.chat),
	}
}

func (w *window) serverText() string {
	var buffer [64]uint16
	procGetWindowText.Call(w.server, uintptr(unsafe.Pointer(&buffer)), uintptr(len(buffer)))
	return strings.TrimSpace(syscall.UTF16ToString(buffer[:]))
}

func (w *window) save() {
	// O endereço é validado antes de gravar qualquer coisa: se estiver errado, a
	// janela fica aberta para o jogador corrigir.
	address := w.serverText()
	if address != w.shownIP && address != "" {
		normalized, err := normalizeAddress(address)
		if err != nil {
			messageBox(err.Error())
			return
		}
		address = normalized
	}
	w.cfg.apply(w.collect())
	if err := w.cfg.save(w.path); err != nil {
		messageBox(err.Error())
		return
	}
	if address != w.shownIP {
		if err := saveServerAddress(w.dir, address); err != nil {
			messageBox(err.Error())
			return
		}
		w.changed = true
	}
	w.close()
}

// close reabilita o dono antes de destruir, para o foco voltar para ele.
func (w *window) close() {
	procEnableWindow.Call(w.owner, 1)
	procDestroyWindow.Call(w.hwnd)
}

func (w *window) proc(hwnd, message, wParam, lParam uintptr) uintptr {
	switch message {
	case wmCommand:
		switch wParam & 0xFFFF {
		case idOK:
			w.save()
			return 0
		case idCancel:
			w.close()
			return 0
		}
	case wmHScroll:
		for _, s := range []slider{w.bright, w.sound, w.music} {
			if s.handle == lParam {
				s.refresh()
			}
		}
		return 0
	case wmClose:
		w.close()
		return 0
	case wmDestroy:
		w.done = true
		return 0
	}
	result, _, _ := procDefWindowProc.Call(hwnd, message, wParam, lParam)
	return result
}

func (w *window) create() error {
	controls := initCommonControls{classes: iccBarClasses}
	controls.size = uint32(unsafe.Sizeof(controls))
	procInitCommonControls.Call(uintptr(unsafe.Pointer(&controls)))

	w.instance, _, _ = procGetModuleHandle.Call(0)
	w.font, _, _ = procGetStockObject.Call(defaultGUIFont)
	icon, _, _ := procLoadIcon.Call(w.instance, mainIconID)
	cursor, _, _ := procLoadCursor.Call(0, idcArrow)
	class := wndClassEx{
		proc:       settingsCallback,
		instance:   w.instance,
		icon:       icon,
		iconSmall:  icon,
		cursor:     cursor,
		background: colorBtnFace + 1,
		className:  text(windowClass),
	}
	class.size = uint32(unsafe.Sizeof(class))
	if !settingsRegistered {
		if atom, _, err := procRegisterClassEx.Call(uintptr(unsafe.Pointer(&class))); atom == 0 {
			return fmt.Errorf("não foi possível criar a janela: %v", err)
		}
		settingsRegistered = true
	}

	bounds := rect{right: clientWidth, bottom: clientHeight}
	procAdjustWindowRect.Call(uintptr(unsafe.Pointer(&bounds)), wsMainWindow, 0)
	width, height := int(bounds.right-bounds.left), int(bounds.bottom-bounds.top)
	screenWidth, _, _ := procGetSystemMetrics.Call(smCXScreen)
	screenHeight, _, _ := procGetSystemMetrics.Call(smCYScreen)

	var err error
	w.hwnd, _, err = procCreateWindowEx.Call(wsExControl,
		uintptr(unsafe.Pointer(class.className)), uintptr(unsafe.Pointer(text(windowTitle))),
		wsMainWindow, (screenWidth-uintptr(width))/2, (screenHeight-uintptr(height))/2,
		uintptr(width), uintptr(height), w.owner, 0, w.instance, 0)
	if w.hwnd == 0 {
		return fmt.Errorf("não foi possível criar a janela: %v", err)
	}
	w.build()
	procShowWindow.Call(w.hwnd, swShow)
	return nil
}

// A janela pode abrir várias vezes e o Windows nunca libera um callback, por isso
// existe só um, que atende a janela aberta no momento.
var (
	activeSettings     *window
	settingsRegistered bool
	settingsCallback   = syscall.NewCallback(func(hwnd, message, wParam, lParam uintptr) uintptr {
		return activeSettings.proc(hwnd, message, wParam, lParam)
	})
)

// runSettings abre as opções presa à janela dona e volta quando elas fecham.
// O resultado diz se o endereço do servidor foi trocado.
func runSettings(owner uintptr, cfg *config, path, dir string) (bool, error) {
	w := &window{cfg: cfg, path: path, owner: owner, dir: dir}
	activeSettings = w
	if err := w.create(); err != nil {
		return false, err
	}
	procEnableWindow.Call(owner, 0)
	var m msg
	for !w.done {
		result, _, _ := procGetMessage.Call(uintptr(unsafe.Pointer(&m)), 0, 0, 0)
		if int32(result) <= 0 {
			procPostQuitMessage.Call(m.wParam) // A janela principal também precisa ver.
			break
		}
		if handled, _, _ := procIsDialogMessage.Call(w.hwnd, uintptr(unsafe.Pointer(&m))); handled != 0 {
			continue
		}
		procTranslateMessage.Call(uintptr(unsafe.Pointer(&m)))
		procDispatchMessage.Call(uintptr(unsafe.Pointer(&m)))
	}
	return w.changed, nil
}
