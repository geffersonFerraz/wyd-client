#pragma once

#include "SControl.h"
#include "GeomObject.h"
#include "Structures.h"
#include "Basedef.h"
#include "TMFont2.h"
#include <functional>

enum class TMEGRIDTYPE
{
    GRID_DEFAULT = 0,
    GRID_SHOP = 1,
    GRID_CARGO = 2,
    GRID_SELL = 3,
    GRID_SKILLM = 4,
    GRID_SKILLB = 5,
    GRID_TRADEMY = 6,
    GRID_TRADEINV = 7,
    GRID_TRADENONE = 8,
    GRID_TRADEOP = 9,
    GRID_TRADEINV2 = 10,
    GRID_TRADEMY2 = 11,
    GRID_ITEMMIX = 12,
    GRID_TRADEINV3 = 13,
    GRID_ITEMMIX4 = 14,
    GRID_TRADEINV6 = 15,
    GRID_TRADEINV8 = 16,
    GRID_CUBEBOX = 17,
    GRID_QUICKSLOAT1 = 18,
    GRID_QUICKSLOAT2 = 19,
    GRID_QUICKSLOAT3 = 20,
    GRID_QUICKSLOAT4 = 21,
    GRID_QUICKSLOAT5 = 22,
    GRID_DELETE = 23,
    GRID_ITEMMIXRESULT = 24,
    GRID_ITEMMIXNEED = 25,
    GRID_MISSION_RESULT = 26,
    GRID_MISSION_NEED = 27,
    GRID_MISSION_NEEDLIST = 28,
    GRID_LEILAOSTORE = 29,
    GRID_LEILAOSTORE_BUY = 30,
    GRID_LEILAOSTORE_DEL_ITEM = 31, 
    GRID_PREMIUMSTORE = 32,
    GRID_DROPLIST = 33,
    GRID_SHOWONLY = 55,
};

static int g_pItemGridXY[8][2] =
{
  { 1, 1 },
  { 1, 2 },
  { 1, 3 },
  { 1, 4 },
  { 2, 1 },
  { 2, 2 },
  { 2, 3 },
  { 2, 4 }
};

class SGridControlItem;
class SGridControl : public SPanel
{
public:
    SGridControl(unsigned int inTextureSetIndex, int inRowGridCount, int inColumnGridCount, float inX, float inY, float inWidth, float inHeight, TMEITEMTYPE type); 
    ~SGridControl();
    int OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY) override;
    void FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag) override;
    int CanItAdd(int* bFilledBuffer, int inCellIndexX, int inCellIndexY, int inCellWidth, int inCellHeight);
    int AddItem(SGridControlItem* ipNewItem, int inCellIndexX, int inCellIndexY);
    int AddSkillItem(SGridControlItem* ipNewItem, int inCellIndexX, int inCellIndexY);
    int SetItem(SGridControlItem* ipNewItem, int inCellIndexX, int inCellIndexY);
    IVector2 AddItemInEmpty(SGridControlItem* ipNewItem);
    IVector2 CanAddItemInEmpty(int nWidth, int nHeight);
    int CanChangeItem(SGridControlItem* ipNewItem, int inCellIndexX, int inCellIndexY, int bOnlyCheck);
    SGridControlItem* PickupItem(int inCellIndexX, int inCellIndexY);
    SGridControlItem* PickupAtItem(int inCellIndexX, int inCellIndexY);
    SGridControlItem* PickupItem(SGridControlItem* ipItem);
    SGridControlItem* SelectItem(int inCellIndexX, int inCellIndexY);
    SGridControlItem* GetItem(int inCellIndexX, int inCellIndexY);
    SGridControlItem* GetAtItem(int inCellIndexX, int inCellIndexY);
    SGridControlItem* GetItem(int nCount);
    int OnKeyDownEvent(unsigned int iKeyCode);
    void Empty();
    void SetStoreQuantities(SGridControlItem* grid);
    void CompactGrid();
    void SwapItems(int index1, int index2);
    void SortItems(std::function<bool(SGridControlItem*, SGridControlItem*)> compare);

    int GetIndexFromCoordinates(int x, int y);

    // Visual 7.502: as paginas internas continuam existindo; so a vista e o clique mudam.
    // Inventario: 60 slots em grade 9x7. Bau: grade 9x14, 126 slots. Loja: grade 9x13.
    bool IsClassicInv() const { return m_dwControlID >= 67072 && m_dwControlID <= 67075; }
    bool IsClassicCargo() const { return m_dwControlID >= 67328 && m_dwControlID <= 67331; }
    bool IsClassicShop() const { return m_dwControlID == 65694; }
    bool IsClassicList() const { return IsClassicInv() || IsClassicCargo() || IsClassicShop(); }
    int ClassicColumns() const { return 9; }
    int ClassicRows() const { return IsClassicInv() ? 7 : (IsClassicCargo() ? 14 : 13); }
    int ClassicPageSize() const { return IsClassicInv() ? 15 : 40; }
    int ClassicPage() const { return IsClassicInv() ? (int)m_dwControlID - 67072 : (IsClassicCargo() ? (int)m_dwControlID - 67328 : 0); }
    bool GetClassicView(int nCellX, int nCellY, int& nView) const;
    void LayoutClassicShop();
    static void ClassicItemSize(int nItemIndex, int& nWidth, int& nHeight);
    bool IsClassicCarry() const { return IsClassicInv() || IsClassicCargo(); }
    static SGridControlItem* ClassicItemAt(bool bCargo, int nViewX, int nViewY);
    static bool ClassicCanPlace(bool bCargo, SGridControlItem* pMoving, int nSlot);
    static int ClassicFirstFit(bool bCargo, SGridControlItem* pMoving);
    void SetVisible(int bVisible) override;
    void RemoveItemAt(int x, int y);
    short CheckType(TMEITEMTYPE eType, TMEGRIDTYPE eGridType);
    short CheckPos(TMEITEMTYPE eType);
    void BuyItem(int nCellX, int nCellY);
    int TradeItem(int nCellX, int nCellY);
    int SellItem(int nCellX, int nCellY, unsigned int dwFlags, unsigned int wParam);
    int SellItem2();
    void SwapItem(int nCellX, int nCellY, int nCellVWidth, int nCellVHeight, STRUCT_ITEM* pItem);
    int MouseOver(int nCellX, int nCellY, int bPtInRect);
    void RButton(int nCellX, int nCellY, int bPtInRect);
    int SetItemOnGrid(STRUCT_ITEM* item, int CellIndexX, int CellIndexY);
    void UpdateCapsuleInfo(int nIndex);
    char AutoSellShowPrice(char* Price);
    char LeilaoStoreShowPrice(char* Price);

    std::string FormatPrice(long long price);
    char automove(int nCellX, int nCellY);
    int Check_ItemRightClick(int nType, int nItemSIndex);
  
private:
    void UpdateDescPanelHeight();

public:
    TMEITEMTYPE m_eItemType;
    int m_nRowGridCount;
    int m_nColumnGridCount;
    int* m_pbFilled;
    GeomControl* m_GCGrid;
    int m_bDrawGrid;
    int m_nNumItem;
    SGridControlItem* m_pItemList[128];
    TMEGRIDTYPE m_eGridType;
    unsigned int m_dwMerchantID;
    unsigned int m_dwLastBuyTime;
    GeomControl m_GCEnable;
    unsigned int m_dwEnableColor;
    IVector2 m_vecPickupedPos;
    IVector2 m_vecPickupedSize;
    long long m_nTradeMoney;
    int m_nLeilaoStoreTipo = 0;
    unsigned int m_dwLastSortTime;
    int _nCellX;
    int _nCellY;
    unsigned int _dwFlags;
    unsigned int _wParam;

public:
    static inline short m_sLastMouseOverIndex{-1};
    static SGridControlItem* m_pLastMouseOverItem;
    static SGridControlItem* m_pLastAttachedItem;
    static SGridControlItem* m_pSellItem;
    static int m_bNeedUpdate;
    static char* m_szParamString[49];
};

class SGridControlItem : public S3DObj
{
public:
    SGridControlItem(SGridControl* pParent, STRUCT_ITEM* pItem, float inX, float inY);
    ~SGridControlItem();

    virtual void SelectThis(int bSelect);
    virtual int IsSelect();
    virtual SGridControl* GetGridControl();
    virtual void SetGridControl(SGridControl* pGridControl);
    virtual STRUCT_ITEM* GetItem();
    void FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag) override;
    int PtInItem(int inPosX, int inPosY);
    int PtAtItem(int inPosX, int inPosY);

public:
    int m_nCellIndexX;
    int m_nCellIndexY;
    int m_nCellWidth;
    int m_nCellHeight;
    int m_bSelect;
    SGridControl* m_pGridControl;
    GeomControl m_GCEnable;
    GeomControl m_GCText;
    TMFont2 m_Font;
    STRUCT_ITEM* m_pItem;
    float m_fTimer;
    // Visual 7.502: celulas ocupadas na grade antiga da loja (w <= 0: nao coube).
    int m_nViewX = 0;
    int m_nViewY = 0;
    int m_nViewW = 1;
    int m_nViewH = 1;
};
