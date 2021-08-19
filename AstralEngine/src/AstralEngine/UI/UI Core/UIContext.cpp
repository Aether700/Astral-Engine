#include "aepch.h"
#include "UIContext.h"
#include "Algorithms/Algorithms.h"
#include "AstralEngine/Core/Time.h"

#define DOCKING_SPLITTER_SIZE 2.0f

namespace AstralEngine
{
    //some random constant, exact value doesn't matter
    constexpr unsigned int ViewportDefaultID = 0x11111111;
	UIContext* uiContext;

    /* CRC (Cyclic Redundancy Check)
       CRC needs a lookup table (not cache friendly)
       Although the code to generate the table is simple and shorter than the table itself, using a const table allows us to easily:
      - avoid an unnecessary branch/memory tap, - keep the HashStr functions usable by static constructors, - make it thread-safe.
    */
    static const size_t CrcLookupTable[256] =
    {
        0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
        0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
        0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
        0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
        0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
        0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
        0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
        0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
        0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
        0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
        0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
        0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
        0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
        0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
        0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
        0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
    };

    //extends DockNodeFlags
    enum class PrivateDockNodeFlags
    {
        //A dockspace is a node that occupy space within an existing user window. 
        //Otherwise the node is floating and create its own window.
        DockSpace = 1 << 10,  

        //The central node has 2 main properties: 
        //stay visible when empty, only use "remaining" spaces from its neighbor.
        CentralNode = 1 << 11,  
        
        //no tab bar at all not even the little triangle prompt to bring it back
        NoTabBar = 1 << 12,  

        HiddenTabBar = 1 << 13,          
        NoWindowMenuButton = 1 << 14,  
        NoCloseButton = 1 << 15,  
 
        //prevents any further docking but preserves the current nodes
        NoDocking = 1 << 16,  
        NoDockingSplitMe = 1 << 17,  
        NoDockingSplitOther = 1 << 18, 
        NoDockingOverMe = 1 << 19, 
        NoDockingOverOther = 1 << 20,  
        NoResizeX = 1 << 21,  
        NoResizeY = 1 << 22,  
        SharedFlagsInheritMask = ~0,
        NoResizeFlagsMask = (int)DockNodeFlags::NoResize | NoResizeX | NoResizeY,
        
        LocalFlagsMask = (int)DockNodeFlags::NoSplit | NoResizeFlagsMask | (int)DockNodeFlags::AutoHideTabBar 
            | DockSpace | CentralNode | NoTabBar | HiddenTabBar | NoWindowMenuButton | NoCloseButton | NoDocking,

        // When splitting those flags are moved to the inheriting child, never duplicated
        LocalFlagsTransferMask = LocalFlagsMask & ~DockSpace,  

        SavedFlagsMask = NoResizeFlagsMask | DockSpace | CentralNode | NoTabBar | HiddenTabBar 
            | NoWindowMenuButton | NoCloseButton | NoDocking
    };


    //extends TabItemFlags
    enum class PrivateTabItemFlags
    {
        // Track whether p_open was set or not (we'll need this info on the next frame to recompute ContentWidth during layout)
        NoCloseButton = 1 << 20,  
        
        // [Docking] Trailing tabs with the _Unsorted flag will be sorted based on the DockOrder of their Window.
        Unsorted = 1 << 21,  
        
        // [Docking] Display tab shape for docking preview (height is adjusted slightly to compensate for the yet missing tab bar)
        Preview = 1 << 22   
    };

    //forward declarations
    static void DockNodeRemoveWindow(DockNode* node, UIWindow* window, unsigned int saveDockID);
    static void DockNodeAddTabBar(DockNode* node);
    static void DockNodeUpdateVisibleFlag(DockNode* node);

    void SetWindowPos(UIWindow* window, const Vector2& pos, UICondition cond);
    void SetWindowSize(UIWindow* window, const Vector2& size, UICondition cond);


    bool operator==(const IntPtr& lhs, const IntPtr& rhs)
    {
        return lhs.i == rhs.i && lhs.ptr == rhs.ptr;
    }
    
    bool operator!=(const IntPtr& lhs, const IntPtr& rhs)
    {
        return !(lhs == rhs);
    }

    DockNode* DockNodeGetRootNode(DockNode* node) 
    { 
        while (node->parentNode != nullptr)
        {
            node = node->parentNode; 
        }
        return node;
    }

    static DockNode* DockContextFindNodeByID(UIContext* ctx, unsigned int id)
    {
        return (DockNode*)ctx->dockingContext.nodes[id].ptr;
    }

    Viewport* GetMainViewport()
    {
        UIContext& g = *uiContext;
        return g.viewports[0];
    }

    TabItem* TabBarFindTabByID(TabBar* tabBar, unsigned int tabID)
    {
        if (tabID != 0)
        {
            for (size_t i = 0; i < tabBar->tabs.GetCount(); i++)
            {
                if (tabBar->tabs[i].id == tabID)
                {
                    return &tabBar->tabs[i];
                }
            }
        }
        return nullptr;
    }

    
    void TabBarAddTab(TabBar* tabBar, TabItemFlags tab_flags, UIWindow* window)
    {
        UIContext& g = *uiContext;
        AE_CORE_ASSERT(TabBarFindTabByID(tabBar, window->id) == nullptr, "");
        AE_CORE_ASSERT(g.currTabBar != tabBar, "");  // Can't work while the tab bar is active as our tab doesn't have an X offset yet, in theory we could/should test something like (tab_bar->CurrFrameVisible < g.FrameCount) but we'd need to solve why triggers the commented early-out assert in BeginTabBarEx() (probably dock node going from implicit to explicit in same frame)

        TabItem newTab;
        newTab.id = window->id;
        newTab.flags = tab_flags;
        // Required so BeginTabBar() doesn't ditch the tab
        newTab.lastFrameVisible = tabBar->currFrameVisible;

        /* needed?
        if (newTab.lastFrameVisible == -1)
        {
            newTab.lastFrameVisible = g.frameCount - 1;
        }
        */

        // Required so tab bar layout can compute the tab width before tab submission
        newTab.window = window;
        tabBar->tabs.Add(newTab);
    }


    void TabBarRemoveTab(TabBar* tabBar, unsigned int tabID)
    {
        TabItem* tab = TabBarFindTabByID(tabBar, tabID);
        if (tab != nullptr)
        {
            tabBar->tabs.Remove(*tab);
        }

        if (tabBar->visibleTabID == tabID)
        {
            tabBar->visibleTabID = 0;
        }

        if (tabBar->selectedTabID == tabID)
        {
            tabBar->selectedTabID = 0;
        }

        if (tabBar->nextSelectedTabID == tabID)
        {
            tabBar->nextSelectedTabID = 0;
        }
    }

    void UpdateWindowParentAndRootLinks(UIWindow* window, UIWindowFlags flags, UIWindow* parentWindow)
    {
        window->parentWindow = parentWindow;
        window->rootWindow = window->rootWindowDockStop = window->rootWindowForTitleBarHighlight
            = window->rootWindowForNav = window;
        if (parentWindow != nullptr && (int)(flags & UIWindowFlags::ChildWindow) && (int)(flags & UIWindowFlags::Tooltip) == 0)
        {
            window->rootWindow = parentWindow->rootWindow;
            if (!window->dockIsActive && (int)(parentWindow->flags & UIWindowFlags::DockNodeHost) == 0)
            {
                window->rootWindowDockStop = parentWindow->rootWindowDockStop;
            }
        }

        if (parentWindow && (int)(flags & UIWindowFlags::Modal) == 0
            && (int)(flags & (UIWindowFlags::ChildWindow | UIWindowFlags::Popup)) != 0)
        {
            window->rootWindowForTitleBarHighlight = parentWindow->rootWindowForTitleBarHighlight;
        }

        while ((int)(window->rootWindowForNav->flags & UIWindowFlags::NavFlattened) != 0)
        {
            AE_CORE_ASSERT(window->rootWindowForNav->parentWindow != nullptr, "");
            window->rootWindowForNav = window->rootWindowForNav->parentWindow;
        }
    }

    static void DockNodeRemoveTabBar(DockNode* node)
    {
        if (node->tabBar == NULL)
        {
            return;
        }

        delete node->tabBar;
        node->tabBar = nullptr;
    }

    static void DockNodeMoveChildNodes(DockNode* destNode, DockNode* srcNode)
    {
        AE_CORE_ASSERT(destNode->windows.GetCount() == 0, "");
        destNode->childNodes[0] = srcNode->childNodes[0];
        destNode->childNodes[1] = srcNode->childNodes[1];

        if (destNode->childNodes[0] != nullptr)
        {
            destNode->childNodes[0]->parentNode = destNode;
        }

        if (destNode->childNodes[1] != nullptr)
        {
            destNode->childNodes[1]->parentNode = destNode;
        }

        destNode->splitAxis = srcNode->splitAxis;
        destNode->sizeRef = srcNode->sizeRef;
        srcNode->childNodes[0] = srcNode->childNodes[1] = nullptr;
    }

    static void DockNodeAddWindow(DockNode* node, UIWindow* window, bool addToTabBar)
    {
        UIContext& g = *uiContext;
        //(void)g; needed?

        if (window->dockNode != nullptr)
        {
            // Can overwrite an existing window->DockNode (e.g. pointing to a disabled DockSpace node)
            AE_CORE_ASSERT(window->dockNode->id != node->id, "");
            DockNodeRemoveWindow(window->dockNode, window, 0);
        }

        AE_CORE_ASSERT(window->dockNode == nullptr || window->dockNodeAsHost == nullptr, "");

        node->windows.Add(window);
        node->wantHiddenTabBarUpdate = true;
        window->dockNode = node;
        window->dockID = node->id;
        window->dockIsActive = (node->windows.GetCount() > 1);
        window->dockTabWantClose = false;

        /*If more than 2 windows appeared on the same frame, we'll create a new hosting DockNode from the point of
          the second window submission. Then we need to hide the first window (after its been output) otherwise it
          would be visible as a standalone window for one frame.
        */
        if (node->hostWindow == nullptr && node->windows.GetCount() == 2 && node->windows[0]->wasActive == false)
        {
            node->windows[0]->hidden = true;
            node->windows[0]->hideWindowSkipItems = 1;
        }

        // When reactivating a node with one or two loose window, the window pos/size/viewport are authoritative over the node storage.
        // In particular it is important we init the viewport from the first window so we don't create two viewports and drop one.
        if (node->hostWindow == nullptr && node->IsFloatingNode())
        {
            if (node->authorityForPos == DockDataAuthority::Auto)
            {
                node->authorityForPos = DockDataAuthority::Window;
            }

            if (node->authorityForSize == DockDataAuthority::Auto)
            {
                node->authorityForSize = DockDataAuthority::Window;
            }

            if (node->authorityForViewport == DockDataAuthority::Auto)
            {
                node->authorityForViewport = DockDataAuthority::Window;
            }
        }

        // Add to tab bar if requested
        if (addToTabBar)
        {
            if (node->tabBar == nullptr)
            {
                DockNodeAddTabBar(node);
                node->tabBar->selectedTabID = node->tabBar->nextSelectedTabID = node->selectedTabID;

                // Add existing windows
                for (size_t i = 0; i < node->windows.GetCount() - 1; i++)
                {
                    TabBarAddTab(node->tabBar, TabItemFlags::None, node->windows[i]); //definition not found by visual studio
                }
            }
            TabBarAddTab(node->tabBar, (TabItemFlags)PrivateTabItemFlags::Unsorted, window);
        }

        DockNodeUpdateVisibleFlag(node);

        // Update this without waiting for the next time we Begin() in the window, so our host window will have the proper title bar color on its first frame.
        if (node->hostWindow != nullptr)
        {
            UpdateWindowParentAndRootLinks(window,
                (UIWindowFlags)((UIWindowFlags)window->flags | UIWindowFlags::ChildWindow), node->hostWindow);
        }
    }


    static void DockNodeMoveWindows(DockNode* destNode, DockNode* srcNode)
    {
        // Insert tabs in the same orders as currently ordered (node->Windows isn't ordered)
        AE_CORE_ASSERT(srcNode != nullptr && destNode != nullptr && destNode != srcNode, "");
        TabBar* srcTabBar = srcNode->tabBar;

        if (srcTabBar != nullptr)
        {
            AE_CORE_ASSERT(srcNode->windows.GetCount() == srcNode->tabBar->tabs.GetCount(), "");
        }

        // If the destNode is empty we can just move the entire tab bar (to preserve selection, scrolling, etc.)
        bool moveTabBar = (srcTabBar != nullptr) && (destNode->tabBar == nullptr);

        if (moveTabBar)
        {
            destNode->tabBar = srcNode->tabBar;
            srcNode->tabBar = nullptr;
        }

        for (int n = 0; n < srcNode->windows.GetCount(); n++)
        {
            UIWindow* window = srcTabBar ? srcTabBar->tabs[n].window : srcNode->windows[n];
            window->dockNode = nullptr;
            window->dockIsActive = false;
            DockNodeAddWindow(destNode, window, !moveTabBar);
        }

        srcNode->windows.Clear();

        if (!moveTabBar && srcNode->tabBar)
        {
            if (destNode->tabBar)
                destNode->tabBar->selectedTabID = srcNode->tabBar->selectedTabID;
            DockNodeRemoveTabBar(srcNode);
        }
    }

    static void DockSettingsRenameNodeReferences(unsigned int oldNodeID, unsigned int newNodeID)
    {
        UIContext& g = *uiContext;
        for (int i = 0; i < g.windows.GetCount(); i++)
        {
            UIWindow* window = g.windows[i];
            if (window->dockID == oldNodeID && window->dockNode == nullptr)
            {
                window->dockID = newNodeID;
            }
        }

        //// FIXME-OPT: We could remove this loop by storing the index in the map (check to do that change from dear ImGui?)
        for (WindowSettings* settings = g.settingsWindows.begin(); settings != nullptr;
            settings = g.settingsWindows.NextChunk(settings))
        {
            if (settings->dockID == oldNodeID)
            {
                settings->dockID = newNodeID;
            }
        }
    }

    static void DockNodeApplyPosSizeToWindows(DockNode* node)
    {
        for (size_t i = 0; i < node->windows.GetCount(); i++)
        {
            // We don't assign directly to Pos because it can break the calculation of SizeContents on next frame
            SetWindowPos(node->windows[i], node->pos, UICondition::Always);

            SetWindowSize(node->windows[i], node->size, UICondition::Always);
        }
    }

    void DockNodeTreeMerge(UIContext* ctx, DockNode* parentNode, DockNode* mergeLeadChild)
    {
        // When called from DockContextProcessUndockNode() it is possible that one of the child is NULL.
        DockNode* child0 = parentNode->childNodes[0];
        DockNode* child1 = parentNode->childNodes[1];
        AE_CORE_ASSERT(child0 != nullptr || child1 != nullptr, "");
        AE_CORE_ASSERT(mergeLeadChild == child0 || mergeLeadChild == child1, "");
        if ((child0 != nullptr && child0->windows.GetCount() > 0) || (child1 != nullptr && child1->windows.GetCount() > 0))
        {
            AE_CORE_ASSERT(parentNode->tabBar == nullptr, "");
            AE_CORE_ASSERT(parentNode->windows.GetCount() == 0, "");
        }

        Vector2 backupLastExplicitSize = parentNode->sizeRef;
        DockNodeMoveChildNodes(parentNode, mergeLeadChild);
        if (child0 != nullptr)
        {
            DockNodeMoveWindows(parentNode, child0); // Generally only 1 of the 2 child node will have windows
            DockSettingsRenameNodeReferences(child0->id, parentNode->id);
        }

        if (child1)
        {
            DockNodeMoveWindows(parentNode, child1);
            DockSettingsRenameNodeReferences(child1->id, parentNode->id);
        }

        DockNodeApplyPosSizeToWindows(parentNode);
        parentNode->authorityForPos = parentNode->authorityForSize = parentNode->authorityForViewport = DockDataAuthority::Auto;
        parentNode->visibleWindow = mergeLeadChild->visibleWindow;
        parentNode->sizeRef = backupLastExplicitSize;

        // Flags transfer

        // Preserve Dockspace flag
        parentNode->localFlags = parentNode->localFlags & (DockNodeFlags)(int)~PrivateDockNodeFlags::LocalFlagsTransferMask;
        parentNode->localFlags = parentNode->localFlags | (child0 ? child0->localFlags : (DockNodeFlags)0)
            & (DockNodeFlags)(int)PrivateDockNodeFlags::LocalFlagsTransferMask;
        parentNode->localFlags = parentNode->localFlags | (child1 ? child1->localFlags : (DockNodeFlags)0)
            & (DockNodeFlags)(int)PrivateDockNodeFlags::LocalFlagsTransferMask;

        if (child0 != nullptr)
        {
            ctx->dockingContext.nodes[child0->id].ptr = nullptr;
            delete child0;
        }

        if (child1 != nullptr)
        {
            ctx->dockingContext.nodes[child1->id].ptr = nullptr;
            delete child1;
        }
    }


    static void DockContextRemoveNode(UIContext* ctx, DockNode* node, bool mergeSiblingIntoParentNode)
    {
        UIContext& g = *ctx;
        DockContext* dc = &ctx->dockingContext;

        AE_CORE_ASSERT(DockContextFindNodeByID(ctx, node->id) == node, "");
        AE_CORE_ASSERT(node->childNodes[0] == nullptr && node->childNodes[1] == nullptr, "");
        AE_CORE_ASSERT(node->windows.GetCount() == 0, "");

        if (node->hostWindow)
        {
            node->hostWindow->dockNodeAsHost = nullptr;
        }

        DockNode* parentNode = node->parentNode;
        const bool merge = (mergeSiblingIntoParentNode && parentNode != nullptr);

        if (merge)
        {
            AE_CORE_ASSERT(parentNode->childNodes[0] == node || parentNode->childNodes[1] == node, "");
            DockNode* siblingNode = (parentNode->childNodes[0] == node ? parentNode->childNodes[1] : parentNode->childNodes[0]);
            DockNodeTreeMerge(&g, parentNode, siblingNode);
        }
        else
        {
            for (int n = 0; parentNode && n < (int)(sizeof(parentNode->childNodes) / sizeof(DockNode*)); n++)
            {
                if (parentNode->childNodes[n] == node)
                {
                    node->parentNode->childNodes[n] = nullptr;
                }
            }

            dc->nodes[node->id].ptr = nullptr;
            delete node;
        }
    }

    static void DockNodeUpdateVisibleFlag(DockNode* node)
    {
        // Update visibility flag
        bool isVisible = (node->parentNode == nullptr) ? node->IsDockSpace() : node->IsCentralNode();
        isVisible |= (node->windows.GetCount() > 0);
        isVisible |= (node->childNodes[0] && node->childNodes[0]->isVisible);
        isVisible |= (node->childNodes[1] && node->childNodes[1]->isVisible);
        node->isVisible = isVisible;
    }

    static void DockNodeRemoveWindow(DockNode* node, UIWindow* window, unsigned int saveDockID)
    {
        UIContext& g = *uiContext;
        AE_CORE_ASSERT(window->dockNode == node, "");
        AE_CORE_ASSERT(saveDockID == 0 || saveDockID == node->id, "");

        window->dockNode = NULL;
        window->dockIsActive = window->dockTabWantClose = false;
        window->dockID = saveDockID;

        // Update immediately
        UpdateWindowParentAndRootLinks(window, (UIWindowFlags)(window->flags & ~UIWindowFlags::ChildWindow), nullptr);

        // Remove window
        bool erased = false;
        for (int n = 0; n < node->windows.GetCount(); n++)
        {
            if (node->windows[n] == window)
            {
                node->windows.Remove(n);
                erased = true;
                break;
            }
        }

        AE_CORE_ASSERT(erased, "");

        if (node->visibleWindow == window)
        {
            node->visibleWindow = nullptr;
        }

        // Remove tab and possibly tab bar
        node->wantHiddenTabBarUpdate = true;
        if (node->tabBar)
        {
            TabBarRemoveTab(node->tabBar, window->id); //function definition not found by visual studio
            const int tabCountThresholdForTabBar = node->IsCentralNode() ? 1 : 2;
            if (node->windows.GetCount() < tabCountThresholdForTabBar)
            {
                DockNodeRemoveTabBar(node);
            }
        }

        if (node->windows.GetCount() == 0 && !node->IsCentralNode() && !node->IsDockSpace() && window->dockID != node->id)
        {
            // Automatic dock node delete themselves if they are not holding at least one tab
            DockContextRemoveNode(&g, node, true);
            return;
        }

        if (node->windows.GetCount() == 1 && !node->IsCentralNode() && node->hostWindow != nullptr)
        {
            UIWindow* remainingWindow = node->windows[0];
            if (node->hostWindow->viewportOwned && node->IsRootNode())
            {
                // Transfer viewport back to the remaining loose window
                AE_CORE_ASSERT(node->hostWindow->viewport->window == node->hostWindow, "");
                node->hostWindow->viewport->window = remainingWindow;
                node->hostWindow->viewport->id = remainingWindow->id;
            }
            remainingWindow->collapsed = node->hostWindow->collapsed;
        }

        // Update visibility immediately is required so the DockNodeUpdateRemoveInactiveChilds() 
        //processing can reflect changes up the tree
        DockNodeUpdateVisibleFlag(node);
    }

    static void DockNodeAddTabBar(DockNode* node)
    {
        AE_CORE_ASSERT(node->tabBar == nullptr, "Provided node already has a tab bar");
        node->tabBar = new TabBar();
    }

    static inline bool IsPowerOfTwo(int v)
    {
        //do bitwise manipulation to check if v is a **power** of two?
        return v != 0 && (v & (v - 1)) == 0;
    }

    static void ApplyWindowSettings(UIWindow* window, WindowSettings* settings)
    {
        Viewport* mainViewport = GetMainViewport();
        window->viewportPos = mainViewport->pos;
        if (settings->viewportID)
        {
            window->viewportID = settings->viewportID;
            window->viewportPos = Vector2(settings->viewportPos.x, settings->viewportPos.y);
        }
        
        window->pos = Vector2((float)(int)(settings->pos.x + window->viewportPos.x), 
            (float)(int)(settings->pos.y + window->viewportPos.y));
        
        if (settings->size.x > 0 && settings->size.y > 0)
        {
           window->size = window->sizeFull = Vector2((float)(int)settings->size.x, (float)(int)settings->size.y);
        }
        
        window->collapsed = settings->collapsed;
        window->dockID = settings->dockID;
        window->dockOrder = settings->dockOrder;
    }

    UIWindow* FindWindowByID(unsigned int id)
    {
        UIContext& g = *uiContext;
        return (UIWindow*)g.windowsByID[id].ptr;
    }

    WindowSettings* FindWindowSettings(unsigned int id)
    {
        UIContext& context = *uiContext;
        for (WindowSettings* settings = context.settingsWindows.begin();
            settings != nullptr; settings = context.settingsWindows.NextChunk(settings))
        {
            if (settings->id == id)
            {
                return settings;
            }
        }
        return nullptr;
    }

    WindowSettings* CreateNewWindowSettings(const char* name)
    {
        UIContext& context = *uiContext;

        //keep?//////////////////////////////////////////
#if !IMGUI_DEBUG_INI_SETTINGS
        // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
        // Preserve the full string when IMGUI_DEBUG_INI_SETTINGS is set to make .ini inspection easier.
        if (const char* p = strstr(name, "###"))
            name = p;
#endif
        //???///////////////////////////////////////////

        const size_t nameLength = strlen(name);

        // Allocate chunk
        const size_t chunkSize = sizeof(WindowSettings) + nameLength + 1;
        WindowSettings* settings = context.settingsWindows.AllocateChunk(chunkSize);
        *settings = WindowSettings();
        settings->id = HashStr(name, nameLength);
        memcpy(settings->GetName(), name, nameLength + 1);   // Store with zero terminator

        return settings;
    }

    WindowSettings* FindOrCreateWindowSettings(const char* name)
    {
        WindowSettings* settings = FindWindowSettings(HashStr(name));
        if (settings != nullptr)
        {
            return settings;
        }
        return CreateNewWindowSettings(name);
    }

    static void WindowSettingsHandlerClearAll(UIContext* context, SettingsHandler*)
    {
        UIContext& ctx = *context;
        for (int i = 0; i != ctx.windows.GetCount(); i++)
            ctx.windows[i]->settingsOffset = -1;
        ctx.settingsWindows.Clear();
    }

    static void* WindowSettingsHandlerReadOpen(UIContext*, SettingsHandler*, const char* name)
    {
        WindowSettings* settings = FindOrCreateWindowSettings(name);
        unsigned id = settings->id;
        *settings = WindowSettings(); // Clear existing if recycling previous entry
        settings->id = id;
        settings->wantApply = true;
        return (void*)settings;
    }

    static void WindowSettingsHandlerReadLine(UIContext*, SettingsHandler*, void* entry, const char* line)
    {
        WindowSettings* settings = (WindowSettings*)entry;
        int x, y;
        int i;
        unsigned int u1;

        //sscanf returns the number of variables filled so we check if all variables were successfully filled
        if (sscanf(line, "Pos=%i,%i", &x, &y) == 2) 
        { 
            settings->pos = Vector2Short((short)x, (short)y); 
        }
        else if (sscanf(line, "Size=%i,%i", &x, &y) == 2) 
        { 
            settings->size = Vector2Short((short)x, (short)y); 
        }
        else if (sscanf(line, "ViewportId=0x%08X", &u1) == 1) 
        { 
            settings->viewportID = u1; 
        }
        else if (sscanf(line, "ViewportPos=%i,%i", &x, &y) == 2) 
        { 
            settings->viewportPos = Vector2Short((short)x, (short)y); 
        }
        else if (sscanf(line, "Collapsed=%d", &i) == 1) 
        { 
            settings->collapsed = (i != 0); 
        }
        else if (sscanf(line, "DockId=0x%X,%d", &u1, &i) == 2) 
        { 
            settings->dockID = u1; 
            settings->dockOrder = (short)i; 
        }
        else if (sscanf(line, "DockId=0x%X", &u1) == 1)
        { 
            settings->dockID = u1; 
            settings->dockOrder = -1; 
        }
        else if (sscanf(line, "ClassId=0x%X", &u1) == 1) 
        { 
            settings->classID = u1; 
        }
    }

    // Apply to existing windows (if any)
    static void WindowSettingsHandlerApplyAll(UIContext* context, SettingsHandler*)
    {
        UIContext& ctx = *context;
        for (WindowSettings* settings = ctx.settingsWindows.begin(); settings != nullptr; settings = ctx.settingsWindows.NextChunk(settings))
        {
            if (settings->wantApply)
            {
                if (UIWindow* window = FindWindowByID(settings->id))
                    ApplyWindowSettings(window, settings);
                settings->wantApply = false;
            }
        }
    }

    static void WindowSettingsHandlerWriteAll(UIContext* context, SettingsHandler* handler, StringBuilder* builder)
    {
        // Gather data from windows that were active during this session
        // (if a window wasn't opened in this session we preserve its settings)
        UIContext& ctx = *context;
        for (int i = 0; i != ctx.windows.GetCount(); i++)
        {
            UIWindow* window = ctx.windows[i];
            if ((int)(window->flags & UIWindowFlags::NoSavedSettings) != 0)
            {
                continue;
            }

            WindowSettings* settings = (window->settingsOffset != -1) ? ctx.settingsWindows.PtrFromOffset(window->settingsOffset) 
                : FindWindowSettings(window->id);
            if (!settings)
            {
                settings = CreateNewWindowSettings(window->name);
                window->settingsOffset = ctx.settingsWindows.OffsetFromPtr(settings);
            }
            AE_CORE_ASSERT(settings->id == window->id, "");
            settings->pos = Vector2Short(window->pos - window->viewportPos);
            settings->size = Vector2Short(window->sizeFull);
            settings->viewportID = window->viewportID;
            settings->viewportPos = Vector2Short(window->viewportPos);
            AE_CORE_ASSERT(window->dockNode == nullptr || window->dockNode->id == window->dockID, "");
            settings->dockID = window->dockID;
            settings->dockOrder = window->dockOrder;
            settings->collapsed = window->collapsed;
        }

        // Write to text buffer
        builder->Reserve(builder->GetCount() + ctx.settingsWindows.GetCount() * 6); // ballpark reserve (?)
        for (WindowSettings* settings = ctx.settingsWindows.begin(); settings != nullptr; 
            settings = ctx.settingsWindows.NextChunk(settings))
        {
            const char* settingsName = settings->GetName();
            builder->Appendf("[%s][%s]\n", handler->typeName, settingsName);
            if (settings->viewportID != 0 && settings->viewportID != 0x11111111) //VIEWPORT_DEFAULT_ID
            {
                builder->Appendf("ViewportPos=%d,%d\n", settings->viewportPos.x, settings->viewportPos.y);
                builder->Appendf("ViewportId=0x%08X\n", settings->viewportID);
            }
            if (settings->pos.x != 0 || settings->pos.y != 0 || settings->viewportID == ViewportDefaultID)
            {
                builder->Appendf("Pos=%d,%d\n", settings->pos.x, settings->pos.y);
            }
  
            if (settings->size.x != 0 || settings->size.y != 0)
            {
                builder->Appendf("Size=%d,%d\n", settings->size.x, settings->size.y);
            }

            builder->Appendf("Collapsed=%d\n", settings->collapsed);
            if (settings->dockID != 0)
            {
                if (settings->dockOrder == -1)
                {
                    builder->Appendf("DockId=0x%08X\n", settings->dockID);
                }
                else
                {
                    builder->Appendf("DockId=0x%08X,%d\n", settings->dockID, settings->dockOrder);
                }
                if (settings->classID != 0)
                {
                    builder->Appendf("ClassId=0x%08X\n", settings->classID);
                }
            }
            builder->Append("\n");
        }
    }

    void SetWindowPos(UIWindow* window, const Vector2& pos, UICondition cond)
    {
        // Test condition (NB: bit 0 is always true) and clear flags for next time
        if ((int)cond != 0 && ((int)window->setWindowPosAllowedFlags & (int)cond) == 0)
        {
            return;
        }

        // Make sure the user doesn't attempt to combine multiple condition flags.
        AE_CORE_ASSERT((int)cond == 0 || IsPowerOfTwo((int)cond), "Do not combine UIConditions as if they were flags"); 

        window->setWindowPosAllowedFlags = (UICondition)
            (window->setWindowPosAllowedFlags & ~(UICondition::Once | UICondition::FirstUseEver | UICondition::Appearing));

        window->setWindowPosVal = Vector2(FLT_MAX, FLT_MAX);

        // Set
        const Vector2 oldPos = window->pos;
        window->pos = Math::Floor(pos);
        Vector2 offset = window->pos - oldPos;
        window->tempData.cursorPos += offset;         // As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
        window->tempData.cursorMaxPos += offset;      // And more importantly we need to offset CursorMaxPos/CursorStartPos this so ContentSize calculation doesn't get affected.
        window->tempData.cursorStartPos += offset;
    }

    void SetWindowSize(UIWindow* window, const Vector2& size, UICondition cond)
    {
        // Test condition (NB: bit 0 is always true) and clear flags for next time
        if ((int)cond != 0 && (int)(window->setWindowSizeAllowedFlags & cond) == 0)
        {
            return;
        }

        // Make sure the user doesn't attempt to combine multiple condition flags.
        AE_CORE_ASSERT((int)cond == 0 || IsPowerOfTwo((int)cond), "Do not combine UIConditions as flags"); 

        window->setWindowSizeAllowedFlags = (UICondition) (window->setWindowSizeAllowedFlags & 
            ~(UICondition::Once | UICondition::FirstUseEver | UICondition::Appearing));

        // Set
        if (size.x > 0.0f)
        {
            window->autoFitFramesX = 0;
            window->sizeFull.x = Math::Floor(size.x);
        }
        else
        {
            window->autoFitFramesX = 2;
            window->autoFitOnlyGrows = false;
        }
        if (size.y > 0.0f)
        {
            window->autoFitFramesY = 0;
            window->sizeFull.y = Math::Floor(size.y);
        }
        else
        {
            window->autoFitFramesY = 2;
            window->autoFitOnlyGrows = false;
        }
    }

    void MarkIniSettingsDirty()
    {
        UIContext& g = *uiContext;
        if (g.settingsDirtyTimer <= 0.0f)
        {
            g.settingsDirtyTimer = g.io.savingRate;
        }
    }

    void DockContextProcessUndockWindow(UIContext* ctx, UIWindow* window, bool clearPersistentDockingRef)
    {
        if (window->dockNode)
        {
            DockNodeRemoveWindow(window->dockNode, window, clearPersistentDockingRef ? 0 : window->dockID);
        }
        else
        {
            window->dockID = 0;
        }
        window->collapsed = false;
        window->dockIsActive = false;
        window->dockTabIsVisible = false;
        MarkIniSettingsDirty();
    }

    void DockBuilderRemoveNodeDockedWindows(unsigned int rootID, bool clearSettingsRefs)
    {
        // Clear references in settings
        UIContext* ctx = uiContext;
        UIContext& g = *ctx;
        if (clearSettingsRefs)
        {
            for (WindowSettings* settings = g.settingsWindows.begin(); settings != nullptr; 
                settings = g.settingsWindows.NextChunk(settings))
            {
                bool wantRemoval = (rootID == 0) || (settings->dockID == rootID);
                if (!wantRemoval && settings->dockID != 0)
                {
                    if (DockNode* node = DockContextFindNodeByID(ctx, settings->dockID))
                    {
                        if (DockNodeGetRootNode(node)->id == rootID)
                        {
                            wantRemoval = true;
                        }
                    }
                }

                if (wantRemoval)
                {
                    settings->dockID = 0;
                }
            }
        }

        // Clear references in windows
        for (int i = 0; i < g.windows.GetCount(); i++)
        {
            UIWindow* window = g.windows[i];
            bool wantRemoval = (rootID == 0) || (window->dockNode && DockNodeGetRootNode(window->dockNode)->id == rootID) 
                || (window->dockNodeAsHost && window->dockNodeAsHost->id == rootID);
            if (wantRemoval)
            {
                const unsigned int backupDockID = window->dockID;
                DockContextProcessUndockWindow(ctx, window, clearSettingsRefs);
                if (!clearSettingsRefs)
                {
                    AE_CORE_ASSERT(window->dockID == backupDockID, "");
                }
            }
        }
    }

    void DockContextQueueNotifyRemovedNode(UIContext* ctx, DockNode* node)
    {
        DockContext* dc = &ctx->dockingContext;
        for (int n = 0; n < dc->requests.GetCount(); n++)
        {
            if (dc->requests[n].dockTargetNode == node) 
            {
                dc->requests[n].type = DockRequestType::None;
            }
        }
    }


    inline int DockNodeGetDepth(const DockNode* node) 
    { 
        int depth = 0; 
        while (node->parentNode != nullptr) 
        { 
            node = node->parentNode; 
            depth++; 
        } 
        return depth; 
    }
    //needs re-work?
    static bool DockNodeComparerDepthMostFirst(const DockNode* lhs, const DockNode* rhs)
    {
        return DockNodeGetDepth(rhs) - DockNodeGetDepth(lhs);
    }

    // root_id = 0 to remove all, root_id != 0 to remove child of given node.
    void DockBuilderRemoveNodeChildNodes(unsigned int rootID)
    {
        UIContext* ctx = uiContext;
        DockContext* dc = &ctx->dockingContext;

        DockNode* rootNode = rootID != 0 ? DockContextFindNodeByID(ctx, rootID) : nullptr;
        if (rootID && rootNode == nullptr)
        {
            return;
        }

        bool hasCentralNode = false;

        DockDataAuthority backupRootNodeAuthorityForPos = rootNode != nullptr ? 
            rootNode->authorityForPos : DockDataAuthority::Auto;

        DockDataAuthority backupRootNodeAuthorityForSize = rootNode != nullptr ?
            rootNode->authorityForSize : DockDataAuthority::Auto;

        // Process active windows
        ADynArr<DockNode*> nodesToRemove;
        for (auto& pair : dc->nodes)
        {
            DockNode* node = (DockNode*)pair.GetElement().ptr;

            if (node != nullptr)
            {
                bool wantRemoval = (rootID == 0) || (node->id != rootID && DockNodeGetRootNode(node)->id == rootID);
                if (wantRemoval)
                {
                    if (node->IsCentralNode())
                    {
                        hasCentralNode = true;
                    }

                    if (rootID != 0)
                    {
                        DockContextQueueNotifyRemovedNode(ctx, node);
                    }

                    if (rootNode)
                    {
                        DockNodeMoveWindows(rootNode, node);
                    }

                    nodesToRemove.Add(node);
                }
            }
        }

        //dockNodeMoveWindows->DockNodeAddWindow will normally set those when reaching 
        //two windows (which is only adequate during interactive merge)
        //Make sure we don't lose our current pos/size. 
        //(FIXME-DOCK: Consider tidying up that code in DockNodeAddWindow instead) make change suggested?
        if (rootNode != nullptr)
        {
            rootNode->authorityForPos = backupRootNodeAuthorityForPos;
            rootNode->authorityForSize = backupRootNodeAuthorityForSize;
        }

        // Apply to settings
        for (WindowSettings* settings = ctx->settingsWindows.begin(); settings != nullptr;
            settings = ctx->settingsWindows.NextChunk(settings))
        {
            unsigned int windowSettingsDockID = settings->dockID;
            if (windowSettingsDockID != 0)
            {
                for (int n = 0; n < nodesToRemove.GetCount(); n++)
                {
                    if (nodesToRemove[n]->id == windowSettingsDockID)
                    {
                        settings->dockID = rootID;
                        break;
                    }
                }
            }
        }

        // Not really efficient, but easier to destroy a whole hierarchy considering DockContextRemoveNode is attempting to merge nodes
        if (nodesToRemove.GetCount() > 1)
        {
            //func below is quick sort check dear imgui for exact signature
            Algorithm::QuickSort(nodesToRemove, (size_t)0, nodesToRemove.GetCount(), DockNodeComparerDepthMostFirst);
        }

        for (int n = 0; n < nodesToRemove.GetCount(); n++)
        {
            DockContextRemoveNode(ctx, nodesToRemove[n], false);
        }

        if (rootID == 0)
        {
            dc->nodes.Clear();
            dc->requests.Clear();
        }
        else if (hasCentralNode)
        {
            rootNode->localFlags = rootNode->localFlags | (DockNodeFlags)(int)PrivateDockNodeFlags::CentralNode;
            rootNode->centralNode = rootNode;
        }
    }

    void DockContextClearNodes(UIContext* ctx, unsigned int rootID, bool clearSettingsRefs)
    {
        AE_CORE_ASSERT(ctx == uiContext, "Invalid UIContext provided");
        DockBuilderRemoveNodeDockedWindows(rootID, clearSettingsRefs);
        DockBuilderRemoveNodeChildNodes(rootID);
    }

    // Clear settings data
    static void DockSettingsHandlerClearAll(UIContext* ctx, SettingsHandler*)
    {
        DockContext* dc = &ctx->dockingContext;
        dc->nodesSettings.Clear();
        DockContextClearNodes(ctx, 0, true);
    }

    static void* DockSettingsHandlerReadOpen(UIContext*, SettingsHandler*, const char* name)
    {
        if (strcmp(name, "Data") != 0)
            return nullptr;
        return (void*)1;
    }

    //returns a c-string with no spaces or tab at the start of the string
    const char* StrSkipBlank(const char* str)
    {
        while (str[0] == ' ' || str[0] == '\t')
        {
            str++;
        }

        return str;
    }

    static DockNodeSettings* DockSettingsFindNodeSettings(UIContext* ctx, unsigned int id)
    {
        //optimize?
        DockContext* dc = &ctx->dockingContext;
        for (int n = 0; n < dc->nodesSettings.GetCount(); n++)
        {
            if (dc->nodesSettings[n].id == id) 
            {
                return &dc->nodesSettings[n];
            }
        }
        return nullptr;
    }


    static void DockSettingsHandlerReadLine(UIContext* ctx, SettingsHandler*, void*, const char* line)
    {
        char c = 0;
        int x = 0, y = 0;
        int r = 0;

        // Parsing, e.g.
        // " DockNode   ID=0x00000001 Pos=383,193 Size=201,322 Split=Y,0.506 "
        // "   DockNode ID=0x00000002 Parent=0x00000001 "
        // Important: this code expect currently fields in a fixed order.
        DockNodeSettings node;
        line = StrSkipBlank(line);
        if (strncmp(line, "DockNode", 8) == 0) 
        { 
            line = StrSkipBlank(line + strlen("DockNode")); 
        }
        else if (strncmp(line, "DockSpace", 9) == 0) 
        { 
            line = StrSkipBlank(line + strlen("DockSpace")); 
            node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::DockSpace;
        }
        else
        {
            return;
        }

        if (sscanf(line, "ID=0x%08X%n", &node.id, &r) == 1) 
        { 
            line += r; 
        }
        else
        {
            return;
        }
        
        if (sscanf(line, " Parent=0x%08X%n", &node.parentNodeID, &r) == 1) 
        { 
            line += r; 
            if (node.parentNodeID == 0)
            {
                return;
            }
        }
        
        if (sscanf(line, " Window=0x%08X%n", &node.parentWindowID, &r) == 1)
        {
            line += r;
            if (node.parentWindowID == 0)
            {
                return;
            }
        }

        if (node.parentNodeID == 0)
        {
            if (sscanf(line, " Pos=%i,%i%n", &x, &y, &r) == 2)
            {
                line += r;
                node.pos = Vector2Short(x, y);
            }
            else
            {
                return;
            }

            if (sscanf(line, " Size=%i,%i%n", &x, &y, &r) == 2)
            {
                line += r;
                node.size = Vector2Short(x, y);
            }
            else
            {
                return;
            }
        }
        else
        {
            if (sscanf(line, " SizeRef=%i,%i%n", &x, &y, &r) == 2)
            { 
                line += r; 
                node.sizeRef = Vector2Short(x, y); 
            }
        }

        if (sscanf(line, " Split=%c%n", &c, &r) == 1) {
            line += r; 
            
            if (c == 'X')
            {
                node.splitAxis = (signed char)Axis::X;
            }
            else if (c == 'Y')
            {
                node.splitAxis = (signed char) Axis::Y;
            }
        }

        if (sscanf(line, " NoResize=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | DockNodeFlags::NoResize;
            }
        }
        
        if (sscanf(line, " CentralNode=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::CentralNode;
            }
        }

        if (sscanf(line, " NoTabBar=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::NoTabBar;
            }
        }

        if (sscanf(line, " HiddenTabBar=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::HiddenTabBar;
            }
        }
        
        if (sscanf(line, " NoWindowMenuButton=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::NoWindowMenuButton;
            }
        }
        
        if (sscanf(line, " NoCloseButton=%d%n", &x, &r) == 1)
        {
            line += r;
            if (x != 0)
            {
                node.flags = node.flags | (DockNodeFlags)(int)PrivateDockNodeFlags::NoCloseButton;
            }
        }
        
        if (sscanf(line, " Selected=0x%08X%n", &node.selectedWindowID, &r) == 1) 
        { 
            line += r; 
        }
        
        if (node.parentNodeID != 0)
        {
            DockNodeSettings* parentSettings = DockSettingsFindNodeSettings(ctx, node.parentNodeID);
            if (parentSettings != nullptr)
            {
                node.depth = parentSettings->depth + 1;
            }
        }
        
        ctx->dockingContext.nodesSettings.Add(node);
    }

    struct DockContextPruneNodeData
    {
        int countWindows, countChildWindows, countChildNodes;
        unsigned int rootID;
        DockContextPruneNodeData() : countWindows(0), countChildWindows(0), countChildNodes(0), rootID(0) { }
    };

    // Remove references stored in ImGuiWindowSettings to the given ImGuiDockNodeSettings
    static void DockSettingsRemoveNodeReferences(unsigned int* nodeIDs, int nodeIDsCount)
    {
        UIContext& g = *uiContext;
        int found = 0;

        //// FIXME-OPT: We could remove this loop by storing the index in the map (do that change?)
        for (WindowSettings* settings = g.settingsWindows.begin(); settings != nullptr; 
            settings = g.settingsWindows.NextChunk(settings))
        {
            for (size_t i = 0; i < nodeIDsCount; i++)
            {
                if (settings->dockID == nodeIDs[i])
                {
                    settings->dockID = 0;
                    settings->dockOrder = -1;
                    if (++found < nodeIDsCount)
                    {
                        break;
                    }
                    return;
                }
            }
        }
    }

    // Garbage collect unused nodes (run once at init time)
    static void DockContextPruneUnusedSettingsNodes(UIContext* ctx)
    {
        UIContext& g = *ctx;
        DockContext* dc = &ctx->dockingContext;
        AE_CORE_ASSERT(g.windows.GetCount() == 0, "");

        AUnorderedMap<unsigned int, DockContextPruneNodeData> pool;

        // Count child nodes and compute RootID
        for (size_t i = 0; i < dc->nodesSettings.GetCount(); i++)
        {
            DockNodeSettings* settings = &dc->nodesSettings[i];
            pool[settings->id].rootID = settings->parentNodeID != 0 ? pool[settings->parentNodeID].rootID : settings->id;
            if (settings->parentNodeID != 0)
            {
                pool[settings->parentNodeID].countChildNodes++;
            }
        }

        // Count reference to dock ids from dockspaces
        // We track the 'auto-DockNode <- manual-Window <- manual-DockSpace' 
        //in order to avoid 'auto-DockNode' being ditched by DockContextPruneUnusedSettingsNodes()
        for (size_t i = 0; i < dc->nodesSettings.GetCount(); i++)
        {
            DockNodeSettings* settings = &dc->nodesSettings[i];
            if (settings->parentWindowID != 0)
            {
                if (WindowSettings* windowSettings = FindWindowSettings(settings->parentWindowID))
                {
                    if (windowSettings->dockID != 0)
                    {
                        if (pool.ContainsKey(windowSettings->dockID))
                        {
                            pool[windowSettings->dockID].countChildNodes++;
                        }
                    }
                }
            }
        }

        // Count reference to dock ids from window settings
        // We guard against the possibility of an invalid .ini file (RootID may point to a missing node)
        for (WindowSettings* settings = g.settingsWindows.begin(); settings != nullptr;
            settings = g.settingsWindows.NextChunk(settings))
        {
            if (settings->dockID != 0)
            {
                if (pool.ContainsKey(settings->dockID))
                {
                    pool[settings->dockID].countWindows++;
                    unsigned int id = (pool[settings->dockID].rootID == settings->dockID) ? pool[settings->dockID].rootID : settings->dockID;
                    if (pool.ContainsKey(id))
                    {
                        pool[id].countChildWindows++;
                    }
                }
            }
        }

        // Prune
        for (size_t i = 0; i < dc->nodesSettings.GetCount(); i++)
        {
            DockNodeSettings* settings = &dc->nodesSettings[i];
            DockContextPruneNodeData* data = &pool[settings->id];
            if (data->countWindows > 1)
            {
                continue;
            }
            DockContextPruneNodeData* dataRoot = (data->rootID == settings->id) ? data : &pool[data->rootID];

            bool remove = false;
            
            // Floating root node with only 1 window
            remove |= (data->countWindows == 1 && settings->parentNodeID == 0 
                && data->countChildNodes == 0 && !(int)(settings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::CentralNode));  
            
            // Leaf nodes with 0 window
            remove |= (data->countWindows == 0 && settings->parentNodeID == 0 && data->countChildNodes == 0);
            
            remove |= (dataRoot->countChildWindows == 0);
            
            if (remove)
            {
                DockSettingsRemoveNodeReferences(&settings->id, 1);
                settings->id = 0;
            }
        }
    }

    unsigned int DockContextGenNodeID(UIContext* ctx)
    {
        // Generate an ID for new node (the exact ID value doesn't matter as long as it is not already used)
        // FIXME-OPT FIXME-DOCK: This is suboptimal, even if the node count is small enough not to be a worry. 
        //We should poke in ctx->Nodes to find a suitable ID faster. (optimize to make faster?)
        unsigned int id = 0x0001;
        while (DockContextFindNodeByID(ctx, id) != nullptr)
        {
            id++;
        }
        return id;
    }


    static DockNode* DockContextAddNode(UIContext* ctx, unsigned int id)
    {
        // Generate an ID for the new node (the exact ID value doesn't matter as long as it is not already used) and add the first window.
        if (id == 0)
        {
            id = DockContextGenNodeID(ctx);
        }
        else
        {
            AE_CORE_ASSERT(DockContextFindNodeByID(ctx, id) == nullptr, "attempting to create a DockNode with an already existing id");
        }
        // We don't set node->LastFrameAlive on construction. Nodes are always created at all time to reflect .ini settings!
        DockNode* node = new DockNode(id);
        ctx->dockingContext.nodes[node->id].ptr = node;
        return node;
    }

    int FormatString(char* buf, size_t buf_size, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        //does a similar parsing to the one seen in printf from C
        //returns the number of characters in the string (assuming a big enough buffer size would have been provided) 
        //or a negative number if an error has occured
        int w = vsnprintf(buf, buf_size, fmt, args);

        va_end(args);
        
        if (buf == nullptr)
        {
            return w;
        }
        
        if (w == -1 || w >= (int)buf_size)
        {
            w = (int)buf_size - 1;
        }
        
        buf[w] = '\0';
        return w;
    }

    static const char* DockNodeGetHostWindowTitle(DockNode* node, char* buf, int bufferSize) 
    { 
        FormatString(buf, bufferSize, "##DockNode_%02X", node->id); 
        return buf; 
    }

    UIWindow* FindWindowByName(const char* name)
    {
        unsigned int id = HashStr(name);
        return FindWindowByID(id);
    }

    static void DockContextBuildNodesFromSettings(UIContext* ctx, DockNodeSettings* nodeSettingsArr, int nodeSettingsCount)
    {
        // Build nodes
        for (size_t i = 0; i < nodeSettingsCount; i++)
        {
            DockNodeSettings* settings = &nodeSettingsArr[i];
            if (settings->id == 0)
                continue;
            DockNode* node = DockContextAddNode(ctx, settings->id);
            node->parentNode = settings->parentNodeID ? DockContextFindNodeByID(ctx, settings->parentNodeID) : nullptr;
            node->pos = Vector2(settings->pos.x, settings->pos.y);
            node->size = Vector2(settings->size.x, settings->size.y);
            node->sizeRef = Vector2(settings->sizeRef.x, settings->sizeRef.y);
            node->authorityForPos = node->authorityForSize = node->authorityForViewport = DockDataAuthority::DockNode;
            if (node->parentNode != nullptr && node->parentNode->childNodes[0] == nullptr)
            {
                node->parentNode->childNodes[0] = node;
            }
            else if (node->parentNode && node->parentNode->childNodes[1] == nullptr)
            {
                node->parentNode->childNodes[1] = node;
            }

            node->selectedTabID = settings->selectedWindowID;
            node->splitAxis = (Axis)settings->splitAxis;
            node->localFlags = node->localFlags | (settings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::SavedFlagsMask);

            // Bind host window immediately if it already exist (in case of a rebuild)
            // This is useful as the RootWindowForTitleBarHighlight links necessary to highlight the currently focused node requires node->HostWindow to be set.
            char hostWindowTitle[20];
            DockNode* rootNode = DockNodeGetRootNode(node);
            node->hostWindow = FindWindowByName(DockNodeGetHostWindowTitle(rootNode, hostWindowTitle, 
                sizeof(hostWindowTitle) / sizeof(char)));
        }
    }

    void DockContextBuildAddWindowsToNodes(UIContext* ctx, unsigned int rootID)
    {
        // Rebind all windows to nodes (they can also lazily rebind but we'll have a visible glitch during the first frame)
        UIContext& g = *ctx;
        for (size_t i = 0; i < g.windows.GetCount(); i++)
        {
            UIWindow* window = g.windows[i];
            if (window->dockID == 0)
            {
                continue;
            }
            
            if (window->dockNode != nullptr)
            {
                continue;
            }

            DockNode* node = DockContextFindNodeByID(ctx, window->dockID);
            AE_CORE_ASSERT(node != nullptr, "");   // This should have been called after DockContextBuildNodesFromSettings()
            if (rootID == 0 || DockNodeGetRootNode(node)->id == rootID)
            {
                DockNodeAddWindow(node, window, true);
            }
        }
    }

    // Recreate nodes based on settings data
    static void DockSettingsHandlerApplyAll(UIContext* ctx, SettingsHandler*)
    {
        // Prune settings at boot time only
        DockContext* dc = &ctx->dockingContext;
        if (ctx->windows.GetCount() == 0)
        {
            DockContextPruneUnusedSettingsNodes(ctx);
        }
        
        DockContextBuildNodesFromSettings(ctx, dc->nodesSettings.GetData(), dc->nodesSettings.GetCount());
        DockContextBuildAddWindowsToNodes(ctx, 0);
    }

    static void DockSettingsHandlerCreateSettingsFromNode(DockContext* dc, DockNode* node, int depth)
    {
        DockNodeSettings nodeSettings;
        AE_CORE_ASSERT(depth < (1 << (sizeof(nodeSettings.depth) << 3)), "");
        nodeSettings.id = node->id;
        nodeSettings.parentNodeID = node->parentNode != nullptr ? node->parentNode->id : 0;
        nodeSettings.parentWindowID = (node->IsDockSpace() && node->hostWindow != nullptr
            && node->hostWindow->parentWindow != nullptr) ? node->hostWindow->parentWindow->id : 0;

        nodeSettings.selectedWindowID = node->selectedTabID;
        nodeSettings.splitAxis = (signed char)(node->IsSplitNode() ? node->splitAxis : Axis::None);
        nodeSettings.depth = (char)depth;
        nodeSettings.flags = (node->localFlags & (DockNodeFlags)(int)PrivateDockNodeFlags::SavedFlagsMask);
        nodeSettings.pos = Vector2Short(node->pos);
        nodeSettings.size = Vector2Short(node->size);
        nodeSettings.sizeRef = Vector2Short(node->sizeRef);
        dc->nodesSettings.Add(nodeSettings);
        if (node->childNodes[0] != nullptr)
        {
            DockSettingsHandlerCreateSettingsFromNode(dc, node->childNodes[0], depth + 1);
        }

        if (node->childNodes[1] != nullptr)
        {
            DockSettingsHandlerCreateSettingsFromNode(dc, node->childNodes[1], depth + 1);
        }
    }

    static void DockSettingsHandlerWriteAll(UIContext* ctx, SettingsHandler* handler, StringBuilder* buf)
    {
        UIContext& g = *ctx;
        DockContext* dc = &ctx->dockingContext;
        if (!(bool)(g.io.configFlags & UIConfigFlags::DockingEnabled))
        {
            return;
        }

        // Gather settings data
        // (unlike our windows settings, because nodes are always built we can do a full rewrite of the SettingsNode buffer)
        
        //dc->nodesSettings.resize(0); needed?
        dc->nodesSettings.Reserve(dc->nodes.GetCount());
        //for (size_t i = 0; i < dc->nodes.GetCount(); i++)
        for (auto& pair : dc->nodes)
        {
            if (DockNode* node = (DockNode*) pair.GetElement().ptr)
            {
                if (node->IsRootNode()) 
                {
                    DockSettingsHandlerCreateSettingsFromNode(dc, node, 0);
                }
            }
        }

        int maxDepth = 0;
        for (size_t i = 0; i < dc->nodesSettings.GetCount(); i++)
        {
            maxDepth = Math::Max((int)dc->nodesSettings[i].depth, maxDepth);
        }

        // Write to text buffer
        buf->Appendf("[%s][Data]\n", handler->typeName);

        for (size_t i = 0; i < dc->nodesSettings.GetCount(); i++)
        {
            const int line_start_pos = buf->GetCount(); (void)line_start_pos;
            const DockNodeSettings* nodeSettings = &dc->nodesSettings[i];
            buf->Appendf("%*s%s%*s", nodeSettings->depth * 2, "", ((bool)(nodeSettings->flags &
                (DockNodeFlags)(int)PrivateDockNodeFlags::DockSpace)) ? "DockSpace" : "DockNode ",
                (maxDepth - nodeSettings->depth) * 2, "");  // Text align nodes to facilitate looking at .ini file

            buf->Appendf(" ID=0x%08X", nodeSettings->id);

            if (nodeSettings->parentNodeID != 0)
            {
                buf->Appendf(" Parent=0x%08X SizeRef=%d,%d", nodeSettings->parentNodeID, nodeSettings->sizeRef.x,
                    nodeSettings->sizeRef.y);
            }
            else
            {
                if (nodeSettings->parentWindowID != 0)
                {
                    buf->Appendf(" Window=0x%08X", nodeSettings->parentWindowID);
                }
                buf->Appendf(" Pos=%d,%d Size=%d,%d", nodeSettings->pos.x, nodeSettings->pos.y, nodeSettings->size.x,
                    nodeSettings->size.y);
            }

            if (nodeSettings->splitAxis != (signed char)Axis::None)
            {
                buf->Appendf(" Split=%c", (nodeSettings->splitAxis == (signed char)Axis::X) ? 'X' : 'Y');
            }

            if ((bool)(nodeSettings->flags & DockNodeFlags::NoResize))
            {
                buf->Appendf(" NoResize=1");
            }

            if ((bool)(nodeSettings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::CentralNode))
            {
                buf->Appendf(" CentralNode=1");
            }

            if ((bool)(nodeSettings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::NoTabBar))
            {
                buf->Appendf(" NoTabBar=1");
            }

            if ((bool)(nodeSettings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::HiddenTabBar))
            {
                buf->Appendf(" HiddenTabBar=1");
            }

            if ((bool)(nodeSettings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::NoWindowMenuButton))
            {
                buf->Appendf(" NoWindowMenuButton=1");
            }

            if ((bool)(nodeSettings->flags & (DockNodeFlags)(int)PrivateDockNodeFlags::NoCloseButton))
            {
                buf->Appendf(" NoCloseButton=1");
            }

            if (nodeSettings->selectedWindowID != 0)
            {
                buf->Appendf(" Selected=0x%08X", nodeSettings->selectedWindowID);
            }
            buf->Appendf("\n");
        }
        buf->Appendf("\n");
    }


    void DockContextInitialize(UIContext* ctx)
    {
        UIContext& g = *ctx;

        // Add .ini handle for persistent docking data
        SettingsHandler iniHandler;
        iniHandler.typeName = "Docking";
        iniHandler.typeHash = HashStr("Docking");
        iniHandler.clearAllFn = DockSettingsHandlerClearAll;
        iniHandler.readInitFn = DockSettingsHandlerClearAll; // Also clear on read
        iniHandler.readOpenFn = DockSettingsHandlerReadOpen;
        iniHandler.readLineFn = DockSettingsHandlerReadLine;
        iniHandler.applyAllFn = DockSettingsHandlerApplyAll;
        iniHandler.writeAllFn = DockSettingsHandlerWriteAll;
        g.settingsHandler.Add(iniHandler);
    }

	UIContext* CreateContext(FontAtlas* atlas)
	{
		UIContext* context = new UIContext(atlas);
		if (uiContext == nullptr)
			SetCurrentContext(context);
		Initialize(context);
		return context;
	}

    //Calls registered handlers (e.g. SettingsHandlerWindow_WriteAll() + custom handlers)
    //to write their data into a text buffer
    const char* GenerateSettingsString()
    {
        UIContext& g = *uiContext;
        g.settingsDirtyTimer = 0.0f;
        g.settingsInitialData.buffer.Clear();
        g.settingsInitialData.buffer.Add(0);
        for (size_t i = 0; i < g.settingsHandler.GetCount(); i++)
        {
            SettingsHandler* handler = &g.settingsHandler[i];
            handler->writeAllFn(&g, handler, &g.settingsInitialData);
        }

        return g.settingsInitialData.c_str();
    }

    void SaveSettingsToDisk(const char* filename)
    {
        UIContext& g = *uiContext;
        g.settingsDirtyTimer = 0.0f;
        if (filename != nullptr)
        {
            return;
        }

        const char* data = GenerateSettingsString();
        
        std::ofstream file(filename);

        if (!file)
        {
            return;
        }

        file << data;
    }

    //needed? (see DestroyPlatformWindows)
    void DestroyPlatformWindow(ViewportPrivate* viewport)
    {
        if (viewport->platformWindowCreated)
        {
            // Don't clear PlatformWindowCreated for the main viewport, as we initially set that up to true in Initialize()
            // The right-er way may be to leave it to the back-end to set this flag all-together, and made the flag public.
            if (viewport->id != ViewportDefaultID)
            {
                viewport->platformWindowCreated = false;
            }
        }
    }

    /* only calls clean up functions from the windows themselves
    * 
    * needed?
    */
    void DestroyPlatformWindows()
    {
        // We call the destroy window on every viewport (including the main viewport, index 0) to give a chance to the back-end
        // to clear any data they may have stored in e.g. PlatformUserData, RendererUserData.
        // It is convenient for the platform back-end code to store something in the main viewport, in order for e.g. the mouse handling
        // code to operator a consistent manner.
        // It is expected that the back-end can handle calls to Renderer_DestroyWindow/Platform_DestroyWindow without
        // crashing if it doesn't have data stored.
        UIContext& g = *uiContext;
        for (int i = 0; i < g.viewports.GetCount(); i++)
        {
            DestroyPlatformWindow((ViewportPrivate*)g.viewports[i]);
        }
    }

    void DockContextShutdown(UIContext* ctx)
    {
        DockContext* dc = &ctx->dockingContext;
        for (auto& pair : dc->nodes)
        {
            delete pair.GetElement().ptr;
        }
    }

    // This function is merely here to free heap allocations.
    void Shutdown(UIContext* context)
    {
        // The fonts atlas can be used prior to calling NewFrame(), 
        //so we clear it even if g.initialized is FALSE (which would happen if we never called NewFrame)
        UIContext& g = *context;
        if (g.io.fonts != nullptr && g.fontAtlasOwnedByContext)
        {
            g.io.fonts->Unlock();
            delete g.io.fonts;
        }
        g.io.fonts = nullptr;

        //cleanup of other data are conditional on actually having initialized the UIContext
        if (!g.initialized)
        {
            return;
        }

        //Save settings (unless we haven't attempted to load them: CreateContext/DestroyContext without a call 
        //to NewFrame shouldn't save an empty file)
        if (g.settingsLoaded && g.io.saveFileName != nullptr)
        {
            UIContext* backupContext = uiContext;
            SetCurrentContext(context);
            SaveSettingsToDisk(g.io.saveFileName);
            SetCurrentContext(backupContext);
        }

        // Destroy platform windows
        UIContext* backupContext = GetCurrentContext();
        SetCurrentContext(context);
        DestroyPlatformWindows();
        SetCurrentContext(backupContext);

        // Shutdown extensions
        DockContextShutdown(&g);

        // Clear everything else
        for (int i = 0; i < g.windows.GetCount(); i++)
        {
            delete g.windows[i];
        }

        g.windows.Clear();
        g.windowFocusOrder.Clear();
        g.windowTempSortBuffer.Clear();
        g.currentWindow = nullptr;
        g.currWindowStack.Clear();
        g.windowsByID.Clear();
        g.navWindow = nullptr;
        g.hoveredWindow = nullptr;
        g.hoveredWindowUnderMovingWindow = nullptr;
        g.activeIDWindow = nullptr;
        g.activeIDPreviousFrameWindow = nullptr;
        g.movingWindow = nullptr;
        g.colorModifiers.Clear();
        g.styleModifiers.Clear();
        g.fontStack.Clear();
        g.openPopupStack.Clear();
        g.beginPopupStack.Clear();

        g.currViewport = nullptr;
        g.mouseViewport = nullptr;
        g.mouseLastHoveredViewport = nullptr;
        
        for (int i = 0; i < g.viewports.GetCount(); i++)
        {
            delete g.viewports[i];
        }

        g.viewports.Clear();

        g.tabBars.Clear();
        g.currentTabBarStack.Clear();
        g.shrinkWidthBuffer.Clear();

        g.clipboardHandlerData.Clear();
        g.menuIDsSubmittedThisFrame.Clear();
        g.inputTextState.ClearFreeMemory();

        g.settingsWindows.Clear();
        g.settingsHandler.Clear();

        //change logFile to use std::ofstream?
        if (g.logFile != nullptr)
        {
            fclose(g.logFile);
            g.logFile = nullptr;
        }

        g.logBuffer.Clear();

        g.initialized = false;
    }


    void DestroyContext(UIContext* ctx)
    {
        if (ctx == nullptr)
        {
            ctx = uiContext;

        }
        
        Shutdown(ctx);
        if (uiContext == ctx)
        {
            SetCurrentContext(NULL);
        }
        
        delete ctx;
    }

	UIContext* GetCurrentContext()
    {
        return uiContext;
    }

	void SetCurrentContext(UIContext* context)
    {
        //check for memory leaks (?)
        uiContext = context;
    }

    void Initialize(UIContext* context)
    {
        UIContext& ctx = *context;
        AE_CORE_ASSERT(!ctx.initialized && !ctx.settingsLoaded, "Context provided has already been initialized");

        // Add .ini handle for ImGuiWindow type
        {
            SettingsHandler iniHandler;
            iniHandler.typeName = "Window";
            iniHandler.typeHash = HashStr("Window");
            iniHandler.clearAllFn = WindowSettingsHandlerClearAll;
            iniHandler.readOpenFn = WindowSettingsHandlerReadOpen;
            iniHandler.readLineFn = WindowSettingsHandlerReadLine;
            iniHandler.applyAllFn = WindowSettingsHandlerApplyAll;
            iniHandler.writeAllFn = WindowSettingsHandlerWriteAll;
            ctx.settingsHandler.Add(iniHandler);
        }

        // Create default viewport
        ViewportPrivate* viewport = new ViewportPrivate();
        viewport->id = ViewportDefaultID;
        viewport->index = 0;
        viewport->platformWindowCreated = true;
        ctx.viewports.Add(viewport);
        
        // Make it accessible in public-facing GetPlatformIO() immediately (before the first call to EndFrame)
        ctx.mainViewport = ctx.viewports[0]; 
        ctx.viewports.Add(ctx.viewports[0]);

        // Extensions
        DockContextInitialize(&ctx);

        ctx.initialized = true;
    }

    Viewport* GetMainViewport()
    {
        UIContext& g = *uiContext;
        return g.viewports[0];
    }

    static void SetWindowConditionAllowedFlags(UIWindow* window, UICondition flags, bool enabled)
    {
        window->setWindowPosAllowedFlags = enabled ? (window->setWindowPosAllowedFlags | flags) 
            : (window->setWindowPosAllowedFlags & ~flags);

        window->setWindowSizeAllowedFlags = enabled ? (window->setWindowSizeAllowedFlags | flags) 
            : (window->setWindowSizeAllowedFlags & ~flags);

        window->setWindowCollapsedAllowedFlags = enabled ? (window->setWindowCollapsedAllowedFlags | flags) 
            : (window->setWindowCollapsedAllowedFlags & ~flags);

        window->setWindowDockAllowedFlags = enabled ? (window->setWindowDockAllowedFlags | flags) 
            : (window->setWindowDockAllowedFlags & ~flags);
    }

    static UIWindow* CreateNewWindow(const char* name, UIWindowFlags flags)
    {
        UIContext& g = *uiContext;

        // Create window the first time
        UIWindow* window = new UIWindow(&g, name);
        window->flags = flags;
        g.windowsByID[window->id].ptr =  window;

        // Default/arbitrary window position. Use SetNextWindowPos() 
        //with the appropriate condition flag to change the initial position of a window.
        Viewport* mainViewport = GetMainViewport();
        window->pos = mainViewport->pos + Vector2(60, 60);
        window->viewportPos = mainViewport->pos;

        // User can disable loading and saving of settings. Tooltip and child windows also don't store settings.
        if (!(bool)(flags & UIWindowFlags::NoSavedSettings))
        {
            if (WindowSettings* settings = FindWindowSettings(window->id))
            {
                // Retrieve settings from .ini file
                window->settingsOffset = g.settingsWindows.OffsetFromPtr(settings);
                SetWindowConditionAllowedFlags(window, UICondition::FirstUseEver, false);
                ApplyWindowSettings(window, settings);
            }
        }
        
        // So first call to CalcContentSize() doesn't return crazy values
        window->tempData.cursorStartPos = window->pos; 
        window->tempData.cursorMaxPos = window->pos; 

        if ((int)(flags & UIWindowFlags::AlwaysAutoResize) != 0)
        {
            window->autoFitFramesX = 2;
            window->autoFitFramesY = 2;
            window->autoFitOnlyGrows = false;
        }
        else
        {
            if (window->size.x <= 0.0f)
            {
                window->autoFitFramesX = 2;
            }
            
            if (window->size.y <= 0.0f)
            {
                window->autoFitFramesY = 2;
            }
            
            window->autoFitOnlyGrows = (window->autoFitFramesX > 0) || (window->autoFitFramesY > 0);
        }

        g.windowFocusOrder.Add(window);

        if ((bool)(flags & UIWindowFlags::NoBringToFrontOnFocus))
        {
            //slow but rare and only once
            g.windows.AddFirst(window);
        }
        else
        {
            g.windows.Add(window);
        }
        return window;
    }


    static void SetWindowConditionAllowFlags(UIWindow* window, UICondition flags, bool enabled)
    {
        window->setWindowPosAllowedFlags = enabled ? (window->setWindowPosAllowedFlags | flags) 
            : (window->setWindowPosAllowedFlags & ~flags);

        window->setWindowSizeAllowedFlags = enabled ? (window->setWindowSizeAllowedFlags | flags) 
            : (window->setWindowSizeAllowedFlags & ~flags);

        window->setWindowCollapsedAllowedFlags = enabled ? (window->setWindowCollapsedAllowedFlags | flags) 
            : (window->setWindowCollapsedAllowedFlags & ~flags);
        
        window->setWindowDockAllowedFlags = enabled ? (window->setWindowDockAllowedFlags | flags) 
            : (window->setWindowDockAllowedFlags & ~flags);
    }

    bool GetWindowAlwaysWantOwnTabBar(UIWindow* window)
    {
        UIContext& g = *uiContext;
        if (g.io.configDockingAlwaysTabBar) //|| window->windowClass.DockingAlwaysTabBar)
        {
            if ((int)(window->flags & (UIWindowFlags::ChildWindow | UIWindowFlags::NoTitleBar | UIWindowFlags::NoDocking)) == 0)
            {
                // We don't support AlwaysTabBar on the fallback/implicit window to avoid unused dock-node overhead/noise
                if (!window->isFallbackWindow)    
                {
                    return true;
                }
            }
        }
        return false;
    }

    // Update Pos/Size for a node hierarchy (don't affect child Windows yet)
    void DockNodeTreeUpdatePosSize(DockNode* node, Vector2 pos, Vector2 size, bool onlyWriteToMarkedNodes)
    {
        // During the regular dock node update we write to all nodes.
        // 'only_write_to_marked_nodes' is only set when turning a node visible mid-frame and we need its size right-away.
        const bool writeToNode = (onlyWriteToMarkedNodes == false) || (node->markedForPosSizeWrite);
        if (writeToNode)
        {
            node->pos = pos;
            node->size = size;
        }

        if (node->IsLeafNode())
        {
            return;
        }

        DockNode* child0 = node->childNodes[0];
        DockNode* child1 = node->childNodes[1];
        Vector2 child0Pos = pos;
        Vector2 child0Size = size;
        Vector2 child1Pos = pos;
        Vector2 child1Size = size;

        if (child0->isVisible && child1->isVisible)
        {
            const float spacing = DOCKING_SPLITTER_SIZE;
            const Axis axis = (Axis)node->splitAxis;
            const float sizeAvail = Math::Max(size[(int)axis] - spacing, 0.0f);

            // Size allocation policy
            // 1) The first 0..WindowMinSize[axis]*2 are allocated evenly to both windows.
            UIContext& g = *uiContext;
            const float sizeMinEach = Math::Floor(Math::Min(sizeAvail, g.style.windowMinSize[(int)axis] * 2.0f) * 0.5f);

            // 2) Process locked absolute size (during a splitter resize we preserve the 
            //child of nodes not touching the splitter edge)
            if (child0->wantLockSizeOnce && !child1->wantLockSizeOnce)
            {
                child0Size[(int)axis] = child0->sizeRef[(int)axis] = Math::Min(sizeAvail - 1.0f, child0->size[(int)axis]);
                child1Size[(int)axis] = child1->sizeRef[(int)axis] = (sizeAvail - child0Size[(int)axis]);
                AE_CORE_ASSERT(child0->sizeRef[(int)axis] > 0.0f && child1->sizeRef[(int)axis] > 0.0f, "");
            }
            else if (child1->wantLockSizeOnce && !child0->wantLockSizeOnce)
            {
                child1Size[(int)axis] = child1->sizeRef[(int)axis] = Math::Min(sizeAvail - 1.0f, child1->size[(int)axis]);
                child0Size[(int)axis] = child0->sizeRef[(int)axis] = (sizeAvail - child1Size[(int)axis]);
                AE_CORE_ASSERT(child0->sizeRef[(int)axis] > 0.0f && child1->sizeRef[(int)axis] > 0.0f, "");
            }
            else if (child0->wantLockSizeOnce && child1->wantLockSizeOnce)
            {
                // FIXME-DOCK: We cannot honor the requested size, so apply ratio.
                // Currently this path will only be taken if code programmatically sets WantLockSizeOnce
                //fix issue?
                float ratio0 = child0Size[(int)axis] / (child0Size[(int)axis] + child1Size[(int)axis]);
                child0Size[(int)axis] = child0->sizeRef[(int)axis] = Math::Floor(sizeAvail * ratio0);
                child1Size[(int)axis] = child1->sizeRef[(int)axis] = (sizeAvail - child0Size[(int)axis]);
                AE_CORE_ASSERT(child0->sizeRef[(int)axis] > 0.0f && child1->sizeRef[(int)axis] > 0.0f, "");
            }

            // 3) If one window is the central node 
            //(~ use remaining space, should be made explicit!), 
            //use explicit size from the other, and remainder for the central node
            else if (child1->IsCentralNode() && child0->sizeRef[(int)axis] != 0.0f)
            {
                child0Size[(int)axis] = Math::Min(sizeAvail - sizeMinEach, child0->sizeRef[(int)axis]);
                child1Size[(int)axis] = (sizeAvail - child0Size[(int)axis]);
            }
            else if (child0->IsCentralNode() && child1->sizeRef[(int)axis] != 0.0f)
            {
                child1Size[(int)axis] = Math::Min(sizeAvail - sizeMinEach, child1->sizeRef[(int)axis]);
                child0Size[(int)axis] = (sizeAvail - child1Size[(int)axis]);
            }
            else
            {
                // 4) Otherwise distribute according to the relative ratio of each SizeRef value
                float splitRatio = child0->sizeRef[(int)axis] / (child0->sizeRef[(int)axis] + child1->sizeRef[(int)axis]);
                child0Size[(int)axis] = Math::Max(sizeMinEach, Math::Floor(sizeAvail * splitRatio + 0.5f));
                child1Size[(int)axis] = (sizeAvail - child0Size[(int)axis]);
            }

            child1Pos[(int)axis] += spacing + child0Size[(int)axis];
        }
        child0->wantLockSizeOnce = false;
        child1->wantLockSizeOnce = false;

        if (child0->isVisible)
        {
            DockNodeTreeUpdatePosSize(child0, child0Pos, child0Size, false);

        }

        if (child1->isVisible)
        {
            DockNodeTreeUpdatePosSize(child1, child1Pos, child1Size, false);
        }
    }


    static DockNode* DockContextBindNodeToWindow(UIContext* ctx, UIWindow* window)
    {
        UIContext& g = *ctx;
        DockNode* node = DockContextFindNodeByID(ctx, window->dockID);
        AE_CORE_ASSERT(window->dockNode == nullptr, "");

        // We should not be docking into a split node (SetWindowDock should avoid this)
        if (node != nullptr && node->IsSplitNode())
        {
            DockContextProcessUndockWindow(ctx, window, true);
            return NULL;
        }

        // Create node
        if (node == nullptr)
        {
            node = DockContextAddNode(ctx, window->dockID);
            node->authorityForPos = node->authorityForSize = node->authorityForViewport = DockDataAuthority::Window;
            node->lastFrameAlive = g.frameCount;
        }

        // If the node just turned visible and is part of a hierarchy, it doesn't have a Size assigned by DockNodeTreeUpdatePosSize() yet,
        // so we're forcing a Pos/Size update from the first ancestor that is already visible (often it will be the root node).
        // If we don't do this, the window will be assigned a zero-size on its first frame, which won't ideally warm up the layout.
        // This is a little wonky because we don't normally update the Pos/Size of visible node mid-frame.
        if (!node->isVisible)
        {
            DockNode* ancestorNode = node;
            while (!ancestorNode->isVisible)
            {
                ancestorNode->isVisible = true;
                ancestorNode->markedForPosSizeWrite = true;
                if (ancestorNode->parentNode!= nullptr)
                    ancestorNode = ancestorNode->parentNode;
            }
            AE_CORE_ASSERT(ancestorNode->size.x > 0.0f && ancestorNode->size.y > 0.0f, "");
            DockNodeTreeUpdatePosSize(ancestorNode, ancestorNode->pos, ancestorNode->size, true);
        }

        // Add window to node
        DockNodeAddWindow(node, window, true);
        AE_CORE_ASSERT(node == window->dockNode, "");
        return node;
    }

    int DockNodeGetTabOrder(UIWindow* window)
    {
        TabBar* tabBar = window->dockNode->tabBar;
        if (tabBar == nullptr)
        {
            return -1;
        }
        
        TabItem* tab = TabBarFindTabByID(tabBar, window->id);
        return tab != nullptr ? tabBar->GetTabOrder(tab) : -1;
    }

    void BeginDocked(UIWindow* window, bool* pOpen)
    {
        UIContext& g = *uiContext;

        const bool auto_dock_node = GetWindowAlwaysWantOwnTabBar(window);
        if (auto_dock_node)
        {
            if (window->dockID == 0)
            {
                AE_CORE_ASSERT(window->dockNode == nullptr, "");
                window->dockID = DockContextGenNodeID(uiContext);
            }
        }
        else
        {
            // Calling SetNextWindowPos() undock windows by default (by setting PosUndock)
            bool want_undock = false;
            want_undock |= (int)(window->flags & UIWindowFlags::NoDocking) != 0;
            //want_undock |= (g.nextWindowData.flags & ImGuiNextWindowDataFlags_HasPos) && (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) && g.NextWindowData.PosUndock;
            if (want_undock)
            {
                DockContextProcessUndockWindow(uiContext, window, true);
                return;
            }
        }

        // Bind to our dock node
        DockNode* node = window->dockNode;
        if (node != nullptr)
            AE_CORE_ASSERT(window->dockID == node->id, "");
        if (window->dockID != 0 && node == nullptr)
        {
            node = DockContextBindNodeToWindow(uiContext, window);
            if (node == nullptr)
            {
                return;
            }
        }

        // Undock if our dockspace node disappeared
        // Note how we are testing for LastFrameAlive 
        // and NOT LastFrameActive. A DockSpace node can 
        // be maintained alive while being inactive with DockNodeFlags::KeepAliveOnly.
        if (node->lastFrameAlive < g.frameCount)
        {
            // If the window has been orphaned, transition the docknode to an implicit node processed in DockContextUpdateDocking()
            DockNode* rootNode = DockNodeGetRootNode(node);
            if (rootNode->lastFrameAlive < g.frameCount)
            {
                DockContextProcessUndockWindow(uiContext, window, true);
            }
            else
            {
                window->dockIsActive = true;
                window->dockTabIsVisible = false;
            }
            return;
        }

        // Fast path return. It is common for windows to hold on a persistent DockId but be the only visible window,
        // and never create neither a host window neither a tab bar.
        // FIXME-DOCK: replace ->HostWindow NULL compare with something more explicit (~was initially intended as a first frame test)
        if (node->hostWindow == nullptr)
        {
            window->dockIsActive = (node->state == DockNodeState::HostWindowHiddenBecauseWindowsAreResizing);
            window->dockTabIsVisible = false;
            return;
        }

        // We can have zero-sized nodes (e.g. children of a small-size dockspace)
        AE_CORE_ASSERT(node->hostWindow != nullptr, "");
        AE_CORE_ASSERT(node->IsLeafNode(), "");
        AE_CORE_ASSERT(node->size.x >= 0.0f && node->size.y >= 0.0f, "");
        node->state = DockNodeState::HostWindowVisible;

        // Undock if we are submitted earlier than the host window
        if (window->beginOrderWithinContext < node->hostWindow->beginOrderWithinContext)
        {
            DockContextProcessUndockWindow(uiContext, window, true);
            return;
        }

        // Position/Size window
        //SetNextWindowPos(node->Pos);
        //SetNextWindowSize(node->Size);
        //g.NextWindowData.PosUndock = false; // Cancel implicit undocking of SetNextWindowPos()
        window->dockIsActive = true;
        window->dockTabIsVisible = false;
        if ((bool)(node->sharedFlags & DockNodeFlags::KeepAliveOnly))
        {
            return;
        }

        // When the window is selected we mark it as visible.
        if (node->visibleWindow == window)
        {
            window->dockTabIsVisible = true;
        }

        // Update window flag
        AE_CORE_ASSERT((int)(window->flags & UIWindowFlags::ChildWindow) == 0, "");
        window->flags = window->flags | UIWindowFlags::ChildWindow 
            | UIWindowFlags::AlwaysUseWindowPadding | UIWindowFlags::NoResize;

        if (node->IsHiddenTabBar() || node->IsNoTabBar())
        {
            window->flags = window->flags | UIWindowFlags::NoTitleBar;
        }
        else
        {
            // Clear the NoTitleBar flag in case the user set it: confusingly 
            //enough we need a title bar height so we are correctly offset, but it won't be displayed!
            window->flags = window->flags & ~UIWindowFlags::NoTitleBar;      
        }

        // Save new dock order only if the tab bar has been visible once.
        // This allows multiple windows to be created in the same frame and have their respective dock orders preserved.
        if (node->tabBar != nullptr && node->tabBar->currFrameVisible != -1)
        {
            window->dockOrder = (short)DockNodeGetTabOrder(window);
        }

        if ((node->wantCloseAll || node->wantCloseTabID == window->id) && pOpen != nullptr)
        {
            *pOpen = false;
        }

        // Update ChildId to allow returning from Child to Parent with Escape
        UIWindow* parentWindow = window->dockNode->hostWindow;
        window->childID = parentWindow->GetID(window->name);
    }

    //Save and compare stack sizes on Begin()/End() to detect usage errors
    //Begin() calls this with write=true
    //End() calls this with write=false
    static void ValidateStackSizes(UIWindow* window, bool write)
    {
        UIContext& g = *uiContext;
        short* p = &window->tempData.stackSizeBackup[0];

        // Window stacks
        // NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
        {
            int n = window->idStack.GetCount();
            if (write)
            {
                *p = (short)n;
            }
            else
            {
                // Too few or too many PopID()/TreePop()
                AE_CORE_ASSERT(*p == n, "PushID/PopID or TreeNode/TreePop Mismatch!");   
            }   
            p++;
        }

        {
            int n = window->tempData.groupStack.GetCount();
            if (write)
            {
                *p = (short)n;
            }
            else
            {
                // Too few or too many EndGroup()
                AE_CORE_ASSERT(*p == n, "BeginGroup/EndGroup Mismatch!");                
            }    
            p++;
        }
        // Global stacks
        // For color, style and font stacks there is an incentive to use Push/Begin/Pop/.../End patterns, so we relax our checks a little to allow them.
        
        { 
            int n = g.beginPopupStack.GetCount();
            if (write)
            {
                *p = (short)n;
            }
            else
            {
                // Too few or too many EndMenu()/EndPopup()
                AE_CORE_ASSERT(*p == n, "BeginMenu/EndMenu or BeginPopup/EndPopup Mismatch!");
            }
            p++; 
        }
        
        { 
            int n = g.colorModifiers.GetCount();
            if (write)
            {
                *p = (short)n;
            }
            else
            {
                // Too few or too many PopStyleColor()
                AE_CORE_ASSERT(*p >= n, "PushStyleColor/PopStyleColor Mismatch!");
            }
            p++; 
        }    
        
        { 
            int n = g.styleModifiers.GetCount();
            if (write)
            {
                *p = (short)n;
            } 
            else
            {
                // Too few or too many PopStyleVar()
                AE_CORE_ASSERT(*p >= n, "PushStyleVar/PopStyleVar Mismatch!");
            }         
            p++; 
        }    
        
        { 
            int n = g.fontStack.GetCount();
            if (write)
            {
                *p = (short)n;
            }
            else
            {
                // Too few or too many PopFont()
                AE_CORE_ASSERT(*p >= n, "PushFont/PopFont Mismatch!");
            } 
            p++; 
        }    
        
        //make sure p went through the whole stackSizeBackup array
        AE_CORE_ASSERT(p == window->tempData.stackSizeBackup 
            + sizeof(window->tempData.stackSizeBackup) / sizeof(*window->tempData.stackSizeBackup), "");
    }

    char* CopyCString(char* dest, size_t* destSize, const char* src)
    {
        size_t destBuffSize = destSize ? *destSize : strlen(dest) + 1;
        size_t srcSize = strlen(src) + 1;
        if (destBuffSize < srcSize)
        {
            delete dest;
            dest = new char[srcSize];
            if (destSize != nullptr)
            {
                *destSize = srcSize;
            }
        }
        return (char*)memcpy(dest, (const void*)src, srcSize);
    }

    static Vector2 CalcWindowContentSize(UIWindow* window)
    {
        if (window->collapsed)
        {
            if (window->autoFitFramesX <= 0 && window->autoFitFramesY <= 0)
            {
                return window->contentSize;
            }
        }
        
        if (window->hidden)// && window->HiddenFramesCannotSkipItems == 0 && window->HiddenFramesCanSkipItems > 0)
        {
            return window->contentSize;
        }
        
        Vector2 size;
        size.x = Math::Floor((window->contentSizeExplicit.x != 0.0f) ? window->contentSizeExplicit.x 
            : window->tempData.cursorMaxPos.x - window->tempData.cursorStartPos.x);

        size.y = Math::Floor((window->contentSizeExplicit.y != 0.0f) ? window->contentSizeExplicit.y 
            : window->tempData.cursorMaxPos.y - window->tempData.cursorStartPos.y);
        return size;
    }

    static void SetWindowViewport(UIWindow* window, ViewportPrivate* viewport)
    {
        window->viewport = viewport;
        window->viewportID = viewport->id;
        window->viewportOwned = (viewport->window == window);
    }

    static bool GetWindowAlwaysWantOwnViewport(UIWindow* window)
    {
        // Tooltips and menus are not automatically forced into their own viewport when the NoMerge flag is set, however the multiplication of viewports makes them more likely to protrude and create their own.
        UIContext& g = *uiContext;
        if (g.io.configViewportNoAutoMerge)// || (window->windowClass.ViewportFlagsOverrideSet & ImGuiViewportFlags_NoAutoMerge))
        {
            if ((bool)(g.configFlagsCurrFrame & UIConfigFlags::ViewportEnabled))
            {
                if (!window->dockIsActive)
                {
                    if ((int)(window->flags & (UIWindowFlags::ChildWindow | UIWindowFlags::ChildMenu | UIWindowFlags::Tooltip)) == 0)
                    {
                        if ((int)(window->flags & UIWindowFlags::Popup) == 0 || (int)(window->flags & UIWindowFlags::Modal) != 0)
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    // We typically use Vector2(-FLT_MAX,-FLT_MAX) to denote an invalid mouse position.
    bool IsMousePosValid(const Vector2* mousePos = nullptr)
    {
        // The assert is only to silence a false-positive in XCode Static Analysis.
        // Because GImGui is not dereferenced in every code path, the static analyzer assume that it may be NULL (which it doesn't for other functions).
        AE_CORE_ASSERT(uiContext != nullptr, "UIContext not initialized");
        const float MOUSE_INVALID = -256000.0f;
        Vector2 p = mousePos != nullptr ? *mousePos : uiContext->io.mousePos;
        return p.x >= MOUSE_INVALID && p.y >= MOUSE_INVALID;
    }

    //returns nullptr if no viewport with the provided id found
    Viewport* FindViewportByID(unsigned int id)
    {
        UIContext& g = *uiContext;
        for (int i = 0; i < g.viewports.GetCount(); i++)
        {
            if (g.viewports[i]->id == id)
            {
                return g.viewports[i];
            }
        }
        return nullptr;
    }

    // FIXME: We should ideally refactor the system to call this every frame (we currently don't)
    ViewportPrivate* AddUpdateViewport(UIWindow* window, unsigned int id, const Vector2& pos, 
        const Vector2& size, ViewportFlags flags)
    {
        UIContext& g = *uiContext;
        AE_CORE_ASSERT(id != 0, "");

        if (window != nullptr)
        {
            if (g.movingWindow != nullptr && g.movingWindow->rootWindow == window)
            {
                flags = flags | ViewportFlags::NoInputs; //| ViewportFlags::NoFocusOnAppearing;
            }

            if ((bool)(window->flags & UIWindowFlags::NoMouseInputs) && (bool)(window->flags & UIWindowFlags::NoNavInputs))
            {
                flags = flags | ViewportFlags::NoInputs;
            }

            /*
            if ((bool)(window->flags & UIWindowFlags::NoFocusOnAppearing))
            {
                flags = flags | ViewportFlags::NoFocusOnAppearing;
            }
            */
        }

        ViewportPrivate* viewport = (ViewportPrivate*)FindViewportByID(id);
        if (viewport != nullptr)
        {
            /*
            if (!viewport->platformRequestMove)
                viewport->Pos = pos;
            if (!viewport->PlatformRequestResize)
                viewport->Size = size;
            */
            viewport->flags = flags | (viewport->flags & ViewportFlags::Minimized); // Preserve existing flags
        }
        else
        {
            // New viewport
            viewport = new ViewportPrivate();
            viewport->id = id;
            viewport->index = g.viewports.GetCount();
            viewport->pos = pos;
            viewport->lastPos = pos;
            viewport->size = size;
            viewport->flags = flags;
            //UpdateViewportPlatformMonitor(viewport);
            g.viewports.Add(viewport);

            /* needed?
            // We normally setup for all viewports in NewFrame() but here need to handle the mid-frame creation of a new viewport.
            // We need to extend the fullscreen clip rect so the OverlayDrawList clip is correct for that the first frame
            g.DrawListSharedData.ClipRectFullscreen.x = ImMin(g.DrawListSharedData.ClipRectFullscreen.x, viewport->Pos.x);
            g.DrawListSharedData.ClipRectFullscreen.y = ImMin(g.DrawListSharedData.ClipRectFullscreen.y, viewport->Pos.y);
            g.DrawListSharedData.ClipRectFullscreen.z = ImMax(g.DrawListSharedData.ClipRectFullscreen.z, viewport->Pos.x + viewport->Size.x);
            g.DrawListSharedData.ClipRectFullscreen.w = ImMax(g.DrawListSharedData.ClipRectFullscreen.w, viewport->Pos.y + viewport->Size.y);
            */

            /*
            // Store initial DpiScale before the OS platform window creation, based on expected monitor data.
            // This is so we can select an appropriate font size on the first frame of our window lifetime
            if (viewport->PlatformMonitor != -1)
                viewport->DpiScale = g.PlatformIO.Monitors[viewport->PlatformMonitor].DpiScale;
            */
        }

        viewport->window = window;
        viewport->lastFrameActive = g.frameCount;
        AE_CORE_ASSERT(window == nullptr || viewport->id == window->id, "");

        if (window != nullptr)
        {
            window->viewportOwned = true;
        }

        return viewport;
    }

    void BringWindowToDisplayFront(UIWindow* window)
    {
        UIContext& g = *uiContext;

        //free up currFrontWindow early
        {
            UIWindow* currFrontWindow = g.windows[g.windows.GetCount() - 1];
            if (currFrontWindow == window || currFrontWindow->rootWindow == window)
            {
                return;
            }
        }

        for (int i = g.windows.GetCount() - 2; i >= 0; i--) // We can ignore the top-most window
        {
            if (g.windows[i] == window)
            {
                memmove(&g.windows[i], &g.windows[i + 1], (size_t)(g.windows.GetCount() - i - 1) * sizeof(UIWindow*));
                g.windows[g.windows.GetCount() - 1] = window;
                break;
            }
        }
    }

    static bool UpdateTryMergeWindowIntoHostViewport(UIWindow* window, ViewportPrivate* viewport)
    {
        UIContext& g = *uiContext;

        if (window->viewport == viewport)
        {
            return false;
        }
        else if ((int)(viewport->flags & ViewportFlags::CanHostOtherWindows) == 0)
        {
            return false;
        }
        else if ((int)(viewport->flags & ViewportFlags::Minimized) != 0)
        {
            return false;
        }
        else if (!viewport->GetMainRect().Contains(window->Rect()))
        {
            return false;
        }
        else if (GetWindowAlwaysWantOwnViewport(window))
        {
            return false;
        }

        for (size_t i = 0; i < g.windows.GetCount(); i++)
        {
            UIWindow* windowBehind = g.windows[i];
            if (windowBehind == window)
            {
                break;
            }
            else if (windowBehind->wasActive && windowBehind->viewportOwned &&
                !(bool)(windowBehind->flags & UIWindowFlags::ChildWindow))
            {
                if (windowBehind->viewport->GetMainRect().Overlaps(window->Rect()))
                {
                    return false;
                }
            }
        }

        // Move to the existing viewport, Move child/hosted windows as well (FIXME-OPT: iterate child)
        ViewportPrivate* oldViewport = window->viewport;
        if (window->viewportOwned)
        {
            for (size_t i = 0; i < g.windows.GetCount(); i++)
            {
                if (g.windows[i]->viewport == oldViewport)
                {
                    SetWindowViewport(g.windows[i], viewport);
                }
            }
        }

        SetWindowViewport(window, viewport);
        BringWindowToDisplayFront(window);

        return true;
    }

    static bool UpdateTryMergeWindowIntoHostViewports(UIWindow* window)
    {
        UIContext& g = *uiContext;
        return UpdateTryMergeWindowIntoHostViewport(window, (ViewportPrivate*)g.viewports[0]);
    }

    // FIXME-VIEWPORT: This is all super messy and ought to be clarified or rewritten. (fix problem?)
    static void UpdateSelectWindowViewport(UIWindow* window)
    {
        UIContext& g = *uiContext;
        UIWindowFlags flags = window->flags;
        //window->viewportAllowPlatformMonitorExtend = -1;

        // Restore main viewport if multi-viewport is not supported by the back-end
        ViewportPrivate* mainViewport = (ViewportPrivate*) g.viewports[0];
        if (!(bool)(g.configFlagsCurrFrame & UIConfigFlags::ViewportEnabled))
        {
            SetWindowViewport(window, mainViewport);
            return;
        }
        window->viewportOwned = false;

        // Appearing popups reset their viewport so they can inherit again
        if ((bool)(flags & (UIWindowFlags::Popup | UIWindowFlags::Tooltip)) && window->appearing)
        {
            window->viewport = nullptr;
            window->viewportID = 0;
        }

        /*
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasViewport) == 0)
        {
            // By default inherit from parent window
            if (window->Viewport == NULL && window->ParentWindow && !window->ParentWindow->IsFallbackWindow)
                window->Viewport = window->ParentWindow->Viewport;

            // Attempt to restore saved viewport id (= window that hasn't been activated yet), try to restore the viewport based on saved 'window->ViewportPos' restored from .ini file
            if (window->Viewport == NULL && window->ViewportId != 0)
            {
                window->Viewport = (ImGuiViewportP*)FindViewportByID(window->ViewportId);
                if (window->Viewport == NULL && window->ViewportPos.x != FLT_MAX && window->ViewportPos.y != FLT_MAX)
                    window->Viewport = AddUpdateViewport(window, window->ID, window->ViewportPos, window->Size, ImGuiViewportFlags_None);
            }
        }
        */

        bool lockViewport = false;
       
        /*
        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasViewport)
        {
            // Code explicitly request a viewport
            window->Viewport = (ImGuiViewportP*)FindViewportByID(g.NextWindowData.ViewportId);
            window->ViewportId = g.NextWindowData.ViewportId; // Store ID even if Viewport isn't resolved yet.
            lock_viewport = true;
        }
        else 
        */
        if ((bool)(flags & UIWindowFlags::ChildWindow) || (bool)(flags & UIWindowFlags::ChildMenu))
        {
            // Always inherit viewport from parent window
            window->viewport = window->parentWindow->viewport;
        }
        else if ((bool)(flags & UIWindowFlags::Tooltip))
        {
            window->viewport = g.mouseViewport;
        }
        else if (GetWindowAlwaysWantOwnViewport(window))
        {
            window->viewport = AddUpdateViewport(window, window->id, window->pos, window->size, ViewportFlags::None);
        }
        else if (g.movingWindow && g.movingWindow->rootWindow == window && IsMousePosValid())
        {
            if (window->viewport != nullptr && window->viewport->window == window)
            {
                window->viewport = AddUpdateViewport(window, window->id, window->pos, window->size, ViewportFlags::None);
            }
        }
        else
        {
            // Merge into host viewport?
            // We cannot test window->ViewportOwned as it set lower in the function.
            bool tryToMergeIntoHostViewport = (window->viewport && window == window->viewport->window && g.activeID == 0);
            if (tryToMergeIntoHostViewport)
            {
                UpdateTryMergeWindowIntoHostViewports(window);
            }
        }

        // Fallback to default viewport
        if (window->viewport == nullptr)
            window->viewport = mainViewport;

        // Mark window as allowed to protrude outside of its viewport and into the current monitor
        if (!lockViewport)
        {
            if ((bool)(flags & (UIWindowFlags::Tooltip | UIWindowFlags::Popup)))
            {
                // We need to take account of the possibility that mouse may become invalid.
                // Popups/Tooltip always set ViewportAllowPlatformMonitorExtend so GetWindowAllowedExtentRect() will return full monitor bounds.
                Vector2 mouseRef = (bool)(flags & UIWindowFlags::Tooltip) ? g.io.mousePos
                    : g.beginPopupStack[g.beginPopupStack.GetCount() - 1].openMousePos;
                bool useMouseRef = (g.navDisableHighlight || !g.navDisableMouseHover || g.navWindow == nullptr);
                bool mouseValid = IsMousePosValid(&mouseRef);

                /*
                if ((window->appearing || (bool)(flags & (UIWindowFlags::Tooltip | UIWindowFlags::ChildMenu))) 
                    && (!useMouseRef || mouseValid))
                {
                    window->viewportAllowedPlatformMonitorExtend = FindPlatformMonitorForPos((useMouseRef && mouseValid) 
                        ? mouseRef : NavCalcPreferredRefPos());
                }
                else
                {
                    window->ViewportAllowPlatformMonitorExtend = window->Viewport->PlatformMonitor;
                }
                */
            }
            else if (window->viewport && window != window->viewport->window && window->viewport->window 
                && !(bool)(flags & UIWindowFlags::ChildWindow))
            {
                // When called from Begin() we don't have access to a proper version of the Hidden flag yet, so we replicate this code.
                const bool willBeVisible = (window->dockIsActive && !window->dockTabIsVisible) ? false : true;
                if ((bool)(window->flags & UIWindowFlags::DockNodeHost) 
                    && window->viewport->lastFrameActive < g.frameCount && willBeVisible)
                {
                    // Steal/transfer ownership
                    window->viewport->window = window;
                    window->viewport->id = window->id;
                    window->viewport->lastNameHash = 0;
                }
                else if (!UpdateTryMergeWindowIntoHostViewports(window)) // Merge?
                {
                    // New viewport
                    window->viewport = AddUpdateViewport(window, window->id, window->pos, window->size, ViewportFlags::None);//ViewportFlags::NoFocusOnAppearing);
                }
            }
            /*
            else if (window->viewportAllowPlatformMonitorExtend < 0 && (flags & ImGuiWindowFlags_ChildWindow) == 0)
            {
                // Regular (non-child, non-popup) windows by default are also allowed to protrude
                // Child windows are kept contained within their parent.
                window->ViewportAllowPlatformMonitorExtend = window->Viewport->PlatformMonitor;
            }
            */
        }

        // Update flags
        window->viewportOwned = (window == window->viewport->window);
        window->viewportID = window->viewport->id;
    }



    /* Push a new UIWindow to add widgets to.
       - A default window called "Debug" is automatically stacked at the beginning of every frame 
         so you can use widgets without explicitly calling a Begin/End pair.
       - Begin/End can be called multiple times during the frame with the same window name to append content.
       - The window name is used as a unique identifier to preserve window information across frames 
         (and save rudimentary information to the .ini file).
         You can use the "##" or "###" markers to use the same label with different id, or same id with different label
       - Return false when window is collapsed, so you can early out in your code. 
         You always need to call End() even if false is returned.
       - Passing 'bool* pOpen' displays a Close button on the upper-right corner of the window, 
         the pointed value will be set to false when the button is pressed.
    */ 
    bool Begin(const char* name, bool* pOpen = nullptr, UIWindowFlags flags = UIWindowFlags::None)
    {
        UIContext& g = *uiContext;
        const UIStyle& style = g.style;

        // Window name required
        AE_CORE_ASSERT(name != nullptr && name[0] != '\0', "");

        /*
        // Forgot to call ImGui::NewFrame()
        AE_CORE_ASSERT(g.FithinFrameScope, "");

        // Called ImGui::Render() or ImGui::EndFrame() and haven't called ImGui::NewFrame() again yet
        AE_CORE_ASSERT(g.frameCountEnded != g.frameCount, "");
        */

        // Find or create
        UIWindow* window = FindWindowByName(name);
        const bool windowJustCreated = (window == nullptr);
        if (windowJustCreated)
        {
            window = CreateNewWindow(name, flags);
        }

        // Automatically disable manual moving/resizing when NoInputs is set
        if ((flags & UIWindowFlags::NoInputs) == UIWindowFlags::NoInputs)
        {
            flags = flags | UIWindowFlags::NoMove | UIWindowFlags::NoResize;
        }

        if ((bool)(flags & UIWindowFlags::NavFlattened))
        {
            AE_CORE_ASSERT((bool)(flags & UIWindowFlags::ChildWindow), "");
        }

        const int currentFrame = g.frameCount;
        const bool firstBeginOfTheFrame = (window->lastFrameActive != currentFrame);

        window->isFallbackWindow = g.currWindowStack.GetCount() == 0;// && g.withinFrameScopeWithImplicitWindow);

        // Update the Appearing flag

        // Not using !WasActive because the implicit "Debug" window would always toggle off->on
        bool windowJustActivatedByUser = (window->lastFrameActive < currentFrame - 1);
        //const bool windowJustAppearingAfterHiddenForResize = (window->hiddenFramesCannotSkipItems > 0);
        if ((bool)(flags & UIWindowFlags::Popup))
        {
            UIPopupData& popupRef = g.openPopupStack[g.beginPopupStack.GetCount()];

            // We recycle popups so treat window as activated if popup id changed
            windowJustActivatedByUser |= (window->popupID != popupRef.popupID);
            windowJustActivatedByUser |= (window != popupRef.window);
        }

        window->appearing = windowJustActivatedByUser;//(windowJustActivatedByUser || windowJustAppearingAfterHiddenForResize);
        if (window->appearing)
            SetWindowConditionAllowFlags(window, UICondition::Appearing, true);

        // Update Flags, LastFrameActive, BeginOrderXXX fields
        if (firstBeginOfTheFrame)
        {
            //window->flagsPrevFrame = window->flags;
            window->flags = (UIWindowFlags)flags;
            window->lastFrameActive = currentFrame;
            window->lastTimeActive = Time::GetTime();
            window->beginOrderWithinParent = 0;
            window->beginOrderWithinContext = (short)(g.windowsActiveCount++);
        }
        else
        {
            flags = window->flags;
        }

        // docking
        // (NB: during the frame dock nodes are created, it is possible that (window->DockIsActive == false) 
        // even though (window->DockNode->Windows.Size > 1)
        // Cannot be both
        AE_CORE_ASSERT(window->dockNode == nullptr || window->dockNodeAsHost == nullptr, "");
        /*
        if (g.nextWindowData.Flags & UINextWindowDataFlags::HasDock)
            SetWindowDock(window, g.NextWindowData.DockId, g.NextWindowData.DockCond);
        */
        if (firstBeginOfTheFrame)
        {
            bool hasDockNode = (window->dockID != 0 || window->dockNode != nullptr);
            bool newAutoDockNode = !hasDockNode && GetWindowAlwaysWantOwnTabBar(window);
            if (hasDockNode || newAutoDockNode)
            {
                BeginDocked(window, pOpen);
                flags = window->flags;
                if (window->dockIsActive)
                {
                    AE_CORE_ASSERT(window->dockNode != nullptr, "");
                }

                // Docking currently override constraints
                //g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;
            }
        }

        // Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
        UIWindow* parentWindowInStack = window->dockIsActive ? window->dockNode->hostWindow : g.currWindowStack.IsEmpty() ? nullptr : g.currWindowStack[g.currWindowStack.GetCount() - 1];
        UIWindow* parentWindow = firstBeginOfTheFrame ? ((bool)(flags & (UIWindowFlags::ChildWindow | UIWindowFlags::Popup)) ? parentWindowInStack : nullptr) : window->parentWindow;
        AE_CORE_ASSERT(parentWindow != nullptr || !(bool)(flags & UIWindowFlags::ChildWindow), "");

        // We allow window memory to be compacted so recreate the base stack when needed.
        if (window->idStack.GetCount() == 0)
            window->idStack.Add(window->id);

        // Add to stack
        // We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
        g.currWindowStack.Add(window);
        g.currentWindow = nullptr;
        ValidateStackSizes(window, true);
        if ((bool)(flags & UIWindowFlags::Popup))
        {
            UIPopupData& popupRef = g.openPopupStack[g.beginPopupStack.GetCount()];
            popupRef.window = window;
            g.beginPopupStack.Add(popupRef);
            window->popupID = popupRef.popupID;
        }

        /*
        if (windowJustAppearingAfterHiddenForResize && !(flags & UIWindowFlags::ChildWindow))
            window->navLastIDs[0] = 0;
        */

        // Update ->RootWindow and others pointers (before any possible call to FocusWindow)
        if (firstBeginOfTheFrame)
        {
            UpdateWindowParentAndRootLinks(window, flags, parentWindow);
        }

        // Process SetNextWindow***() calls
        // (FIXME: Consider splitting the HasXXX flags into X/Y components (do optimization?)
        bool windowPosSetByAPI = false;
        bool windowSizeXSetByAPI = false;
        bool windowSizeYSetByAPI = false;

        /*
        if (g.nextWindowData.flags & ImGuiNextWindowDataFlags_HasPos)
        {
            windowPosSetByAPI = (window->setWindowPosAllowedFlags & g.nextWindowData.posCond) != 0;
            if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
            {
                // May be processed on the next frame if this is our first frame and we are measuring size
                // FIXME: Look into removing the branch so everything can go through this same code path for consistency.
                window->setWindowPosVal = g.NextWindowData.PosVal;
                window->setWindowPosPivot = g.NextWindowData.PosPivotVal;
                window->setWindowPosAllowedFlags = window->setWindowPosAllowedFlags
                    & ~(UICondition::Once | UICondition::FirstUseEver | UICondition::Appearing);
            }
            else
            {
                SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
            }
        }

        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize)
        {
            windowSizeXSetByAPI = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
            windowSizeYSetByAPI = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
            SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
        }

        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasScroll)
        {
            if (g.NextWindowData.ScrollVal.x >= 0.0f)
            {
                window->ScrollTarget.x = g.NextWindowData.ScrollVal.x;
                window->ScrollTargetCenterRatio.x = 0.0f;
            }
            if (g.NextWindowData.ScrollVal.y >= 0.0f)
            {
                window->ScrollTarget.y = g.NextWindowData.ScrollVal.y;
                window->ScrollTargetCenterRatio.y = 0.0f;
            }
        }

        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasContentSize)
            window->ContentSizeExplicit = g.NextWindowData.ContentSizeVal;
        else
        */
        if (firstBeginOfTheFrame)
        { 
            window->contentSizeExplicit = Vector2::Zero();
        }

        /*
        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasWindowClass)
            window->WindowClass = g.NextWindowData.WindowClass;
        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasCollapsed)
            SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
        if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasFocus)
            FocusWindow(window);
        */
        
        if (window->appearing)
        {
            SetWindowConditionAllowFlags(window, UICondition::Appearing, false);
        }

        // When reusing window again multiple times a frame, just append content (don't need to setup again)
        if (firstBeginOfTheFrame)
        {
            // Initialize
            // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345) (check to fix (?))
            const bool windowIsChildTooltip = (bool)(flags & UIWindowFlags::ChildWindow) && (bool)(flags & UIWindowFlags::Tooltip); 
            window->active = true;
            window->hasCloseButton = (pOpen != NULL);
            window->clipRectangle = Vector4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
            window->idStack.Reserve(1);
            /*
            window->drawlist->_ResetForNewFrame();

            // Restore buffer capacity when woken from a compacted state, to avoid
            if (window->memoryCompacted)
                GcAwakeTransientWindowBuffers(window);
            */

            // Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
            // The title bar always display the 'name' parameter, so we only update the string storage if it needs to be visible to the end-user elsewhere.
            bool windowTitleVisibleElsewhere = false;
            if ((window->viewport != nullptr && window->viewport->window == window) || (window->dockIsActive))
            {
                windowTitleVisibleElsewhere = true;
            }
            //Window titles visible when using CTRL+TAB
            else if (g.navWindowingListWindow != nullptr && (int)(window->flags & UIWindowFlags::NoNavFocus) == 0)  
            {
                windowTitleVisibleElsewhere = true;
            }

            if (windowTitleVisibleElsewhere && !windowJustCreated && strcmp(name, window->name) != 0)
            {
                size_t bufferLen = (size_t)window->nameBufferLength;
                window->name = CopyCString(window->name, &bufferLen, name);
                window->nameBufferLength = (int)bufferLen;
            }

            // UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

            // Update contents size from last frame for auto-fitting (or use explicit size)
            window->contentSize = CalcWindowContentSize(window);
            
            /*
            if (window->hiddenFramesCanSkipItems > 0)
                window->hiddenFramesCanSkipItems--;
            if (window->hiddenFramesCannotSkipItems > 0)
                window->hiddenFramesCannotSkipItems--;

            // Hide new windows for one frame until they calculate their size
            if (windowJustCreated && (!windowSizeXSetByAPI || !windowSizeYSetByAPI))
                window->hiddenFramesCannotSkipItems = 1;
            */

            // Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
            // We reset Size/ContentSize for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
            if (windowJustActivatedByUser && (int)(flags & (UIWindowFlags::Popup | UIWindowFlags::Tooltip)) != 0)
            {
                //window->hiddenFramesCannotSkipItems = 1;
                if ((bool)(flags & UIWindowFlags::AlwaysAutoResize))
                {
                    if (!windowSizeXSetByAPI)
                    {
                        window->size.x = 0.0f;
                        window->sizeFull.x = 0.0f;
                    }
                

                    if (!windowSizeYSetByAPI)
                    {
                        window->size.y = 0.0f;
                        window->sizeFull.y = 0.f;
                    }
            
                    window->contentSize = Vector2::Zero();
                }
            }

            // SELECT VIEWPORT
            // We need to do this before using any style/font sizes, as viewport with a different DPI may affect font sizes.

            UpdateSelectWindowViewport(window);
            SetCurrentViewport(window, window->Viewport);
            window->FontDpiScale = (g.IO.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts) ? window->Viewport->DpiScale : 1.0f;
            SetCurrentWindow(window);
            flags = window->Flags;

            // LOCK BORDER SIZE AND PADDING FOR THE FRAME (so that altering them doesn't cause inconsistencies)
            // We read Style data after the call to UpdateSelectWindowViewport() which might be swapping the style.

            if (flags & ImGuiWindowFlags_ChildWindow)
                window->WindowBorderSize = style.ChildBorderSize;
            else
                window->WindowBorderSize = ((flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) && !(flags & ImGuiWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
            if (!window->DockIsActive && (flags & ImGuiWindowFlags_ChildWindow) && !(flags & (ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
                window->WindowPadding = ImVec2(0.0f, (flags & ImGuiWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
            else
                window->WindowPadding = style.WindowPadding;

            // Lock menu offset so size calculation can use it as menu-bar windows need a minimum size.
            window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
            window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

            // Collapse window by double-clicking on title bar
            // At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
            if (!(flags & ImGuiWindowFlags_NoTitleBar) && !(flags & ImGuiWindowFlags_NoCollapse) && !window->DockIsActive)
            {
                // We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
                ImRect title_bar_rect = window->TitleBarRect();
                if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
                    window->WantCollapseToggle = true;
                if (window->WantCollapseToggle)
                {
                    window->Collapsed = !window->Collapsed;
                    MarkIniSettingsDirty(window);
                    FocusWindow(window);
                }
            }
            else
            {
                window->Collapsed = false;
            }
            window->WantCollapseToggle = false;

            // SIZE

            // Calculate auto-fit size, handle automatic resize
            const ImVec2 size_auto_fit = CalcWindowAutoFitSize(window, window->ContentSize);
            bool use_current_size_for_scrollbar_x = window_just_created;
            bool use_current_size_for_scrollbar_y = window_just_created;
            if ((flags & ImGuiWindowFlags_AlwaysAutoResize) && !window->Collapsed)
            {
                // Using SetNextWindowSize() overrides ImGuiWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
                if (!window_size_x_set_by_api)
                {
                    window->SizeFull.x = size_auto_fit.x;
                    use_current_size_for_scrollbar_x = true;
                }
                if (!window_size_y_set_by_api)
                {
                    window->SizeFull.y = size_auto_fit.y;
                    use_current_size_for_scrollbar_y = true;
                }
            }
            else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
            {
                // Auto-fit may only grow window during the first few frames
                // We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor ImGuiWindowFlags_AlwaysAutoResize when collapsed.
                if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
                {
                    window->SizeFull.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
                    use_current_size_for_scrollbar_x = true;
                }
                if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
                {
                    window->SizeFull.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
                    use_current_size_for_scrollbar_y = true;
                }
                if (!window->Collapsed)
                    MarkIniSettingsDirty(window);
            }

            // Apply minimum/maximum window size constraints and final size
            window->SizeFull = CalcWindowSizeAfterConstraint(window, window->SizeFull);
            window->Size = window->Collapsed && !(flags & ImGuiWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

            // Decoration size
            const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();

            // POSITION

            // Popup latch its initial position, will position itself when it appears next frame
            if (window_just_activated_by_user)
            {
                window->AutoPosLastDirection = ImGuiDir_None;
                if ((flags & ImGuiWindowFlags_Popup) != 0 && !(flags & ImGuiWindowFlags_Modal) && !window_pos_set_by_api) // FIXME: BeginPopup() could use SetNextWindowPos()
                    window->Pos = g.BeginPopupStack.back().OpenPopupPos;
            }

            // Position child window
            if (flags & ImGuiWindowFlags_ChildWindow)
            {
                IM_ASSERT(parent_window && parent_window->Active);
                window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
                parent_window->DC.ChildWindows.push_back(window);
                if (!(flags & ImGuiWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
                    window->Pos = parent_window->DC.CursorPos;
            }

            const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
            if (window_pos_with_pivot)
                SetWindowPos(window, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot, 0); // Position given a pivot (e.g. for centering)
            else if ((flags & ImGuiWindowFlags_ChildMenu) != 0)
                window->Pos = FindBestWindowPosForPopup(window);
            else if ((flags & ImGuiWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
                window->Pos = FindBestWindowPosForPopup(window);
            else if ((flags & ImGuiWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
                window->Pos = FindBestWindowPosForPopup(window);

            // Late create viewport if we don't fit within our current host viewport.
            if (window->ViewportAllowPlatformMonitorExtend >= 0 && !window->ViewportOwned && !(window->Viewport->Flags & ImGuiViewportFlags_Minimized))
                if (!window->Viewport->GetMainRect().Contains(window->Rect()))
                {
                    // This is based on the assumption that the DPI will be known ahead (same as the DPI of the selection done in UpdateSelectWindowViewport)
                    //ImGuiViewport* old_viewport = window->Viewport;
                    window->Viewport = AddUpdateViewport(window, window->ID, window->Pos, window->Size, ImGuiViewportFlags_NoFocusOnAppearing);

                    // FIXME-DPI
                    //IM_ASSERT(old_viewport->DpiScale == window->Viewport->DpiScale); // FIXME-DPI: Something went wrong
                    SetCurrentViewport(window, window->Viewport);
                    window->FontDpiScale = (g.IO.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts) ? window->Viewport->DpiScale : 1.0f;
                    SetCurrentWindow(window);
                }

            bool viewport_rect_changed = false;
            if (window->ViewportOwned)
            {
                // Synchronize window --> viewport in most situations
                // Synchronize viewport -> window in case the platform window has been moved or resized from the OS/WM
                if (window->Viewport->PlatformRequestMove)
                {
                    window->Pos = window->Viewport->Pos;
                    MarkIniSettingsDirty(window);
                }
                else if (memcmp(&window->Viewport->Pos, &window->Pos, sizeof(window->Pos)) != 0)
                {
                    viewport_rect_changed = true;
                    window->Viewport->Pos = window->Pos;
                }

                if (window->Viewport->PlatformRequestResize)
                {
                    window->Size = window->SizeFull = window->Viewport->Size;
                    MarkIniSettingsDirty(window);
                }
                else if (memcmp(&window->Viewport->Size, &window->Size, sizeof(window->Size)) != 0)
                {
                    viewport_rect_changed = true;
                    window->Viewport->Size = window->Size;
                }

                // The viewport may have changed monitor since the global update in UpdateViewportsNewFrame()
                // Either a SetNextWindowPos() call in the current frame or a SetWindowPos() call in the previous frame may have this effect.
                if (viewport_rect_changed)
                    UpdateViewportPlatformMonitor(window->Viewport);

                // Update common viewport flags
                const ImGuiViewportFlags viewport_flags_to_clear = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration | ImGuiViewportFlags_NoRendererClear;
                ImGuiViewportFlags viewport_flags = window->Viewport->Flags & ~viewport_flags_to_clear;
                const bool is_short_lived_floating_window = (flags & (ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_Popup)) != 0;
                if (flags & ImGuiWindowFlags_Tooltip)
                    viewport_flags |= ImGuiViewportFlags_TopMost;
                if (g.IO.ConfigViewportsNoTaskBarIcon || is_short_lived_floating_window)
                    viewport_flags |= ImGuiViewportFlags_NoTaskBarIcon;
                if (g.IO.ConfigViewportsNoDecoration || is_short_lived_floating_window)
                    viewport_flags |= ImGuiViewportFlags_NoDecoration;

                // For popups and menus that may be protruding out of their parent viewport, we enable _NoFocusOnClick so that clicking on them
                // won't steal the OS focus away from their parent window (which may be reflected in OS the title bar decoration).
                // Setting _NoFocusOnClick would technically prevent us from bringing back to front in case they are being covered by an OS window from a different app,
                // but it shouldn't be much of a problem considering those are already popups that are closed when clicking elsewhere.
                if (is_short_lived_floating_window && (flags & ImGuiWindowFlags_Modal) == 0)
                    viewport_flags |= ImGuiViewportFlags_NoFocusOnAppearing | ImGuiViewportFlags_NoFocusOnClick;

                // We can overwrite viewport flags using ImGuiWindowClass (advanced users)
                // We don't default to the main viewport because.
                if (window->WindowClass.ParentViewportId)
                    window->Viewport->ParentViewportId = window->WindowClass.ParentViewportId;
                else if ((flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_Tooltip)) && parent_window_in_stack)
                    window->Viewport->ParentViewportId = parent_window_in_stack->Viewport->ID;
                else
                    window->Viewport->ParentViewportId = g.IO.ConfigViewportsNoDefaultParent ? 0 : IMGUI_VIEWPORT_DEFAULT_ID;
                if (window->WindowClass.ViewportFlagsOverrideSet)
                    viewport_flags |= window->WindowClass.ViewportFlagsOverrideSet;
                if (window->WindowClass.ViewportFlagsOverrideClear)
                    viewport_flags &= ~window->WindowClass.ViewportFlagsOverrideClear;

                // We also tell the back-end that clearing the platform window won't be necessary, as our window is filling the viewport and we have disabled BgAlpha
                if (!(flags & ImGuiWindowFlags_NoBackground))
                    viewport_flags &= ~ImGuiViewportFlags_NoRendererClear;

                window->Viewport->Flags = viewport_flags;
            }

            // Calculate the range of allowed position for that window (to be movable and visible past safe area padding)
            // When clamping to stay visible, we will enforce that window->Pos stays inside of visibility_rect.
            ImRect viewport_rect(window->Viewport->GetMainRect());
            ImRect viewport_work_rect(window->Viewport->GetWorkRect());
            ImVec2 visibility_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
            ImRect visibility_rect(viewport_work_rect.Min + visibility_padding, viewport_work_rect.Max - visibility_padding);

            // Clamp position/size so window stays visible within its viewport or monitor
            // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
            // FIXME: Similar to code in GetWindowAllowedExtentRect()
            if (!window_pos_set_by_api && !(flags & ImGuiWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
            {
                if (!window->ViewportOwned && viewport_rect.GetWidth() > 0 && viewport_rect.GetHeight() > 0.0f)
                {
                    ClampWindowRect(window, visibility_rect);
                }
                else if (window->ViewportOwned && g.PlatformIO.Monitors.Size > 0)
                {
                    if (window->Viewport->PlatformMonitor == -1)
                    {
                        // Fallback for "lost" window (e.g. a monitor disconnected): we move the window back over the main viewport
                        SetWindowPos(window, g.Viewports[0]->Pos + style.DisplayWindowPadding, ImGuiCond_Always);
                    }
                    else
                    {
                        ImGuiPlatformMonitor& monitor = g.PlatformIO.Monitors[window->Viewport->PlatformMonitor];
                        visibility_rect.Min = monitor.WorkPos + visibility_padding;
                        visibility_rect.Max = monitor.WorkPos + monitor.WorkSize - visibility_padding;
                        ClampWindowRect(window, visibility_rect);
                    }
                }
            }
            window->Pos = ImFloor(window->Pos);

            // Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
            // Large values tend to lead to variety of artifacts and are not recommended.
            if (window->ViewportOwned || window->DockIsActive)
                window->WindowRounding = 0.0f;
            else
                window->WindowRounding = (flags & ImGuiWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

            // For windows with title bar or menu bar, we clamp to FrameHeight(FontSize + FramePadding.y * 2.0f) to completely hide artifacts.
            //if ((window->Flags & ImGuiWindowFlags_MenuBar) || !(window->Flags & ImGuiWindowFlags_NoTitleBar))
            //    window->WindowRounding = ImMin(window->WindowRounding, g.FontSize + style.FramePadding.y * 2.0f);

            // Apply window focus (new and reactivated windows are moved to front)
            bool want_focus = false;
            if (window_just_activated_by_user && !(flags & ImGuiWindowFlags_NoFocusOnAppearing))
            {
                if (flags & ImGuiWindowFlags_Popup)
                    want_focus = true;
                else if ((window->DockIsActive || (flags & ImGuiWindowFlags_ChildWindow) == 0) && !(flags & ImGuiWindowFlags_Tooltip))
                    want_focus = true;
            }

            // Decide if we are going to handle borders and resize grips
            const bool handle_borders_and_resize_grips = (window->DockNodeAsHost || !window->DockIsActive);

            // Handle manual resize: Resize Grips, Borders, Gamepad
            int border_held = -1;
            ImU32 resize_grip_col[4] = {};
            const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // Allow resize from lower-left if we have the mouse cursor feedback for it.
            const float resize_grip_draw_size = IM_FLOOR(ImMax(g.FontSize * 1.35f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
            if (handle_borders_and_resize_grips && !window->Collapsed)
                if (UpdateWindowManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0], visibility_rect))
                    use_current_size_for_scrollbar_x = use_current_size_for_scrollbar_y = true;
            window->ResizeBorderHeld = (signed char)border_held;

            // Synchronize window --> viewport again and one last time (clamping and manual resize may have affected either)
            if (window->ViewportOwned)
            {
                if (!window->Viewport->PlatformRequestMove)
                    window->Viewport->Pos = window->Pos;
                if (!window->Viewport->PlatformRequestResize)
                    window->Viewport->Size = window->Size;
                viewport_rect = window->Viewport->GetMainRect();
            }

            // Save last known viewport position within the window itself (so it can be saved in .ini file and restored)
            window->ViewportPos = window->Viewport->Pos;

            // SCROLLBAR VISIBILITY

            // Update scrollbar visibility (based on the Size that was effective during last frame or the auto-resized Size).
            if (!window->Collapsed)
            {
                // When reading the current size we need to read it after size constraints have been applied.
                // When we use InnerRect here we are intentionally reading last frame size, same for ScrollbarSizes values before we set them again.
                ImVec2 avail_size_from_current_frame = ImVec2(window->SizeFull.x, window->SizeFull.y - decoration_up_height);
                ImVec2 avail_size_from_last_frame = window->InnerRect.GetSize() + window->ScrollbarSizes;
                ImVec2 needed_size_from_last_frame = window_just_created ? ImVec2(0, 0) : window->ContentSize + window->WindowPadding * 2.0f;
                float size_x_for_scrollbars = use_current_size_for_scrollbar_x ? avail_size_from_current_frame.x : avail_size_from_last_frame.x;
                float size_y_for_scrollbars = use_current_size_for_scrollbar_y ? avail_size_from_current_frame.y : avail_size_from_last_frame.y;
                //bool scrollbar_y_from_last_frame = window->ScrollbarY; // FIXME: May want to use that in the ScrollbarX expression? How many pros vs cons?
                window->ScrollbarY = (flags & ImGuiWindowFlags_AlwaysVerticalScrollbar) || ((needed_size_from_last_frame.y > size_y_for_scrollbars) && !(flags & ImGuiWindowFlags_NoScrollbar));
                window->ScrollbarX = (flags & ImGuiWindowFlags_AlwaysHorizontalScrollbar) || ((needed_size_from_last_frame.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar));
                if (window->ScrollbarX && !window->ScrollbarY)
                    window->ScrollbarY = (needed_size_from_last_frame.y > size_y_for_scrollbars) && !(flags & ImGuiWindowFlags_NoScrollbar);
                window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
            }

            // UPDATE RECTANGLES (1- THOSE NOT AFFECTED BY SCROLLING)
            // Update various regions. Variables they depends on should be set above in this function.
            // We set this up after processing the resize grip so that our rectangles doesn't lag by a frame.

            // Outer rectangle
            // Not affected by window border size. Used by:
            // - FindHoveredWindow() (w/ extra padding when border resize is enabled)
            // - Begin() initial clipping rect for drawing window background and borders.
            // - Begin() clipping whole child
            const ImRect host_rect = ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup) && !window_is_child_tooltip) ? parent_window->ClipRect : viewport_rect;
            const ImRect outer_rect = window->Rect();
            const ImRect title_bar_rect = window->TitleBarRect();
            window->OuterRectClipped = outer_rect;
            if (window->DockIsActive)
                window->OuterRectClipped.Min.y += window->TitleBarHeight();
            window->OuterRectClipped.ClipWith(host_rect);

            // Inner rectangle
            // Not affected by window border size. Used by:
            // - InnerClipRect
            // - ScrollToBringRectIntoView()
            // - NavUpdatePageUpPageDown()
            // - Scrollbar()
            window->InnerRect.Min.x = window->Pos.x;
            window->InnerRect.Min.y = window->Pos.y + decoration_up_height;
            window->InnerRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x;
            window->InnerRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y;

            // Inner clipping rectangle.
            // Will extend a little bit outside the normal work region.
            // This is to allow e.g. Selectable or CollapsingHeader or some separators to cover that space.
            // Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
            // Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
            // Affected by window/frame border size. Used by:
            // - Begin() initial clip rect
            float top_border_size = (((flags & ImGuiWindowFlags_MenuBar) || !(flags & ImGuiWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
            window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerRect.Min.x + ImMax(ImFloor(window->WindowPadding.x * 0.5f), window->WindowBorderSize));
            window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerRect.Min.y + top_border_size);
            window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerRect.Max.x - ImMax(ImFloor(window->WindowPadding.x * 0.5f), window->WindowBorderSize));
            window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerRect.Max.y - window->WindowBorderSize);
            window->InnerClipRect.ClipWithFull(host_rect);

            // Default item width. Make it proportional to window size if window manually resizes
            if (window->Size.x > 0.0f && !(flags & ImGuiWindowFlags_Tooltip) && !(flags & ImGuiWindowFlags_AlwaysAutoResize))
                window->ItemWidthDefault = ImFloor(window->Size.x * 0.65f);
            else
                window->ItemWidthDefault = ImFloor(g.FontSize * 16.0f);

            // SCROLLING

            // Lock down maximum scrolling
            // The value of ScrollMax are ahead from ScrollbarX/ScrollbarY which is intentionally using InnerRect from previous rect in order to accommodate
            // for right/bottom aligned items without creating a scrollbar.
            window->ScrollMax.x = ImMax(0.0f, window->ContentSize.x + window->WindowPadding.x * 2.0f - window->InnerRect.GetWidth());
            window->ScrollMax.y = ImMax(0.0f, window->ContentSize.y + window->WindowPadding.y * 2.0f - window->InnerRect.GetHeight());

            // Apply scrolling
            window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
            window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

            // DRAWING

            // Setup draw list and outer clipping rectangle
            IM_ASSERT(window->DrawList->CmdBuffer.Size == 1 && window->DrawList->CmdBuffer[0].ElemCount == 0);
            window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
            PushClipRect(host_rect.Min, host_rect.Max, false);

            // Draw modal or window list full viewport dimming background (for other viewports we'll render them in EndFrame)
            ImGuiWindow* window_window_list = g.NavWindowingListWindow;
            const bool dim_bg_for_modal = (flags & ImGuiWindowFlags_Modal) && window == GetTopMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
            const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && ((window == g.NavWindowingTargetAnim->RootWindow) || (window == window_window_list && window_window_list->Viewport != g.NavWindowingTargetAnim->Viewport));
            if (dim_bg_for_modal || dim_bg_for_window_list)
            {
                const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? ImGuiCol_ModalWindowDimBg : ImGuiCol_NavWindowingDimBg, g.DimBgRatio);
                window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
            }

            // Draw navigation selection/windowing rectangle background
            if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
            {
                ImRect bb = window->Rect();
                bb.Expand(g.FontSize);
                if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
                    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
            }

            // Since 1.71, child window can render their decoration (bg color, border, scrollbars, etc.) within their parent to save a draw call.
            // When using overlapping child windows, this will break the assumption that child z-order is mapped to submission order.
            // We disable this when the parent window has zero vertices, which is a common pattern leading to laying out multiple overlapping child.
            // We also disabled this when we have dimming overlay behind this specific one child.
            // FIXME: More code may rely on explicit sorting of overlapping child window and would need to disable this somehow. Please get in contact if you are affected.
            const bool is_undocked_or_docked_visible = !window->DockIsActive || window->DockTabIsVisible;
            if (is_undocked_or_docked_visible)
            {
                bool render_decorations_in_parent = false;
                if ((flags & ImGuiWindowFlags_ChildWindow) && !(flags & ImGuiWindowFlags_Popup) && !window_is_child_tooltip)
                    if (window->DrawList->CmdBuffer.back().ElemCount == 0 && parent_window->DrawList->VtxBuffer.Size > 0)
                        render_decorations_in_parent = true;
                if (render_decorations_in_parent)
                    window->DrawList = parent_window->DrawList;

                // Handle title bar, scrollbar, resize grips and resize borders
                const ImGuiWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
                const bool title_bar_is_highlight = want_focus || (window_to_highlight && (window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight || (window->DockNode && window->DockNode == window_to_highlight->DockNode)));
                RenderWindowDecorations(window, title_bar_rect, title_bar_is_highlight, handle_borders_and_resize_grips, resize_grip_count, resize_grip_col, resize_grip_draw_size);

                if (render_decorations_in_parent)
                    window->DrawList = &window->DrawListInst;
            }

            // Draw navigation selection/windowing rectangle border
            if (g.NavWindowingTargetAnim == window)
            {
                float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
                ImRect bb = window->Rect();
                bb.Expand(g.FontSize);
                if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
                {
                    bb.Expand(-g.FontSize - 1.0f);
                    rounding = window->WindowRounding;
                }
                window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
            }

            // UPDATE RECTANGLES (2- THOSE AFFECTED BY SCROLLING)

            // Work rectangle.
            // Affected by window padding and border size. Used by:
            // - Columns() for right-most edge
            // - TreeNode(), CollapsingHeader() for right-most edge
            // - BeginTabBar() for right-most edge
            const bool allow_scrollbar_x = !(flags & ImGuiWindowFlags_NoScrollbar) && (flags & ImGuiWindowFlags_HorizontalScrollbar);
            const bool allow_scrollbar_y = !(flags & ImGuiWindowFlags_NoScrollbar);
            const float work_rect_size_x = (window->ContentSizeExplicit.x != 0.0f ? window->ContentSizeExplicit.x : ImMax(allow_scrollbar_x ? window->ContentSize.x : 0.0f, window->Size.x - window->WindowPadding.x * 2.0f - window->ScrollbarSizes.x));
            const float work_rect_size_y = (window->ContentSizeExplicit.y != 0.0f ? window->ContentSizeExplicit.y : ImMax(allow_scrollbar_y ? window->ContentSize.y : 0.0f, window->Size.y - window->WindowPadding.y * 2.0f - decoration_up_height - window->ScrollbarSizes.y));
            window->WorkRect.Min.x = ImFloor(window->InnerRect.Min.x - window->Scroll.x + ImMax(window->WindowPadding.x, window->WindowBorderSize));
            window->WorkRect.Min.y = ImFloor(window->InnerRect.Min.y - window->Scroll.y + ImMax(window->WindowPadding.y, window->WindowBorderSize));
            window->WorkRect.Max.x = window->WorkRect.Min.x + work_rect_size_x;
            window->WorkRect.Max.y = window->WorkRect.Min.y + work_rect_size_y;

            // [LEGACY] Content Region
            // FIXME-OBSOLETE: window->ContentRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
            // Used by:
            // - Mouse wheel scrolling + many other things
            window->ContentRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
            window->ContentRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + decoration_up_height;
            window->ContentRegionRect.Max.x = window->ContentRegionRect.Min.x + (window->ContentSizeExplicit.x != 0.0f ? window->ContentSizeExplicit.x : (window->Size.x - window->WindowPadding.x * 2.0f - window->ScrollbarSizes.x));
            window->ContentRegionRect.Max.y = window->ContentRegionRect.Min.y + (window->ContentSizeExplicit.y != 0.0f ? window->ContentSizeExplicit.y : (window->Size.y - window->WindowPadding.y * 2.0f - decoration_up_height - window->ScrollbarSizes.y));

            // Setup drawing context
            // (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
            window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
            window->DC.GroupOffset.x = 0.0f;
            window->DC.ColumnsOffset.x = 0.0f;
            window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, decoration_up_height + window->WindowPadding.y - window->Scroll.y);
            window->DC.CursorPos = window->DC.CursorStartPos;
            window->DC.CursorPosPrevLine = window->DC.CursorPos;
            window->DC.CursorMaxPos = window->DC.CursorStartPos;
            window->DC.CurrLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
            window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;

            window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
            window->DC.NavLayerCurrentMask = (1 << ImGuiNavLayer_Main);
            window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
            window->DC.NavLayerActiveMaskNext = 0x00;
            window->DC.NavFocusScopeIdCurrent = (flags & ImGuiWindowFlags_ChildWindow) ? parent_window->DC.NavFocusScopeIdCurrent : 0; // -V595
            window->DC.NavHideHighlightOneFrame = false;
            window->DC.NavHasScroll = (window->ScrollMax.y > 0.0f);

            window->DC.MenuBarAppending = false;
            window->DC.MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);
            window->DC.TreeDepth = 0;
            window->DC.TreeJumpToParentOnPopMask = 0x00;
            window->DC.ChildWindows.resize(0);
            window->DC.StateStorage = &window->StateStorage;
            window->DC.CurrentColumns = NULL;
            window->DC.LayoutType = ImGuiLayoutType_Vertical;
            window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : ImGuiLayoutType_Vertical;
            window->DC.FocusCounterRegular = window->DC.FocusCounterTabStop = -1;

            window->DC.ItemWidth = window->ItemWidthDefault;
            window->DC.TextWrapPos = -1.0f; // disabled
            window->DC.ItemFlagsStack.resize(0);
            window->DC.ItemWidthStack.resize(0);
            window->DC.TextWrapPosStack.resize(0);
            window->DC.GroupStack.resize(0);
            window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : ImGuiItemFlags_Default_;
            if (parent_window)
                window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);

            if (window->AutoFitFramesX > 0)
                window->AutoFitFramesX--;
            if (window->AutoFitFramesY > 0)
                window->AutoFitFramesY--;

            // Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
            if (want_focus)
            {
                FocusWindow(window);
                NavInitWindow(window, false);
            }

            // Close requested by platform window
            if (p_open != NULL && window->Viewport->PlatformRequestClose && window->Viewport != GetMainViewport())
            {
                if (!window->DockIsActive || window->DockTabIsVisible)
                {
                    window->Viewport->PlatformRequestClose = false;
                    g.NavWindowingToggleLayer = false; // Assume user mapped PlatformRequestClose on ALT-F4 so we disable ALT for menu toggle. False positive not an issue.
                    IMGUI_DEBUG_LOG_VIEWPORT("Window '%s' PlatformRequestClose\n", window->Name);
                    *p_open = false;
                }
            }

            // Title bar
            if (!(flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive)
                RenderWindowTitleBarContents(window, title_bar_rect, name, p_open);

            // Clear hit test shape every frame
            window->HitTestHoleSize.x = window->HitTestHoleSize.y = 0;

            // Pressing CTRL+C while holding on a window copy its content to the clipboard
            // This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
            // Maybe we can support CTRL+C on every element?
            /*
            if (g.ActiveId == move_id)
                if (g.IO.KeyCtrl && IsKeyPressedMap(ImGuiKey_C))
                    LogToClipboard();
            */

            if (g.IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                // Docking: Dragging a dockable window (or any of its child) turns it into a drag and drop source.
                // We need to do this _before_ we overwrite window->DC.LastItemId below because BeginDockableDragDropSource() also overwrites it.
                if ((g.MovingWindow == window) && (g.IO.ConfigDockingWithShift == g.IO.KeyShift))
                    if ((window->RootWindow->Flags & ImGuiWindowFlags_NoDocking) == 0)
                        BeginDockableDragDropSource(window);

                // Docking: Any dockable window can act as a target. For dock node hosts we call BeginDockableDragDropTarget() in DockNodeUpdate() instead.
                if (g.DragDropActive && !(flags & ImGuiWindowFlags_NoDocking))
                    if (g.MovingWindow == NULL || g.MovingWindow->RootWindow != window)
                        if ((window == window->RootWindow) && !(window->Flags & ImGuiWindowFlags_DockNodeHost))
                            BeginDockableDragDropTarget(window);
            }

            // We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
            // This is useful to allow creating context menus on title bar only, etc.
            if (window->DockIsActive)
            {
                window->DC.LastItemId = window->ID;
                window->DC.LastItemStatusFlags = window->DockTabItemStatusFlags;
                window->DC.LastItemRect = window->DockTabItemRect;
            }
            else
            {
                window->DC.LastItemId = window->MoveId;
                window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? ImGuiItemStatusFlags_HoveredRect : 0;
                window->DC.LastItemRect = title_bar_rect;
            }

#ifdef IMGUI_ENABLE_TEST_ENGINE
            if (!(window->Flags & ImGuiWindowFlags_NoTitleBar))
                IMGUI_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
        }
        else
        {
            // Append
            SetCurrentViewport(window, window->Viewport);
            SetCurrentWindow(window);
        }

        if (!(flags & ImGuiWindowFlags_DockNodeHost))
            PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

        // Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
        if (first_begin_of_the_frame)
            window->WriteAccessed = false;

        window->BeginCount++;
        g.NextWindowData.ClearFlags();

        // When we are about to select this tab (which will only be visible on the _next frame_), flag it with a non-zero HiddenFramesCannotSkipItems.
        // This will have the important effect of actually returning true in Begin() and not setting SkipItems, allowing an earlier submission of the window contents.
        // This is analogous to regular windows being hidden from one frame.
        // It is especially important as e.g. nested TabBars would otherwise generate flicker in the form of one empty frame, or focus requests won't be processed.
        if (window->DockIsActive && !window->DockTabIsVisible)
        {
            if (window->LastFrameJustFocused == g.FrameCount)
                window->HiddenFramesCannotSkipItems = 1;
            else
                window->HiddenFramesCanSkipItems = 1;
        }

        if (flags & ImGuiWindowFlags_ChildWindow)
        {
            // Child window can be out of sight and have "negative" clip windows.
            // Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
            IM_ASSERT((flags & ImGuiWindowFlags_NoTitleBar) != 0 || (window->DockIsActive));
            if (!(flags & ImGuiWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
                if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
                    window->HiddenFramesCanSkipItems = 1;

            // Hide along with parent or if parent is collapsed
            if (parent_window && (parent_window->Collapsed || parent_window->HiddenFramesCanSkipItems > 0))
                window->HiddenFramesCanSkipItems = 1;
            if (parent_window && (parent_window->Collapsed || parent_window->HiddenFramesCannotSkipItems > 0))
                window->HiddenFramesCannotSkipItems = 1;
        }

        // Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
        if (style.Alpha <= 0.0f)
            window->HiddenFramesCanSkipItems = 1;

        // Update the Hidden flag
        window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

        // Update the SkipItems flag, used to early out of all items functions (no layout required)
        bool skip_items = false;
        if (window->Collapsed || !window->Active || window->Hidden)
            if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
                skip_items = true;
        window->SkipItems = skip_items;

        return !skip_items;
    }


    void End()
    {

    }


    // Zero-terminated string hash, with support for ### to reset back to seed value
    // We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
    // Because this syntax is rarely used we are optimizing for the common case.
    // - If we reach ### in the string we discard the hash so far and reset to the seed.
    // - We don't do 'current += 2; continue;' after handling ### to keep the code smaller/faster (measured ~10% diff in Debug build)
    // FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.

    //change hash function?
    unsigned int HashStr(const char* str, size_t length, size_t seed)
    {
        seed = ~seed;
        size_t crc = seed;
        const unsigned char* data = (const unsigned char*)str;
        const size_t* crcTable = CrcLookupTable;
        if (length != 0)
        {
            while (length-- != 0)
            {
                unsigned char c = *data;
                if (c == '#' && length >= 3 && data[1] == '#' && data[2] == '#')
                {
                    crc = seed;
                }
                data++;

                //recall ^ is bitwise XOR
                crc = (crc >> 8) ^ crcTable[crc ^ c];
            }
        }
        else
        {
            unsigned char c = *data;
            while (c != '\0')
            {
                if (c == '#' && data[1] == '#' && data[2] == '#')
                {
                    crc = seed;
                }

                data++;

                crc = (crc >> 8) ^ crcTable[crc ^ c];
            }
        }
        return ~crc;
    }

    float UIWindow::CalcFontSize() const
    {
        UIContext& g = *uiContext;
        float scale = g.fontBaseSize * fontWindowScale * fontDpiScale;
        if (parentWindow != nullptr)
        {
            scale *= parentWindow->fontWindowScale; return scale;
        }
    }

    float UIWindow::TitleBarHeight() const 
    { 
        UIContext& g = *uiContext; 
        if ((int)(flags & UIWindowFlags::NoTitleBar) != 0) 
        {
            return 0.0f;
        }

        return CalcFontSize() + g.style.framePadding.y * 2.0f; 
    }

    float UIWindow::MenuBarHeight() const 
    { 
        UIContext& g = *uiContext; 
        if ((int)(flags & UIWindowFlags::MenuBar) != 0)
        {
            return tempData.menuBarOffset.y + CalcFontSize() + g.style.framePadding.y * 2.0f;
        }
        return 0.0f; 
    }


}