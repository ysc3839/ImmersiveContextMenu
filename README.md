# ImmersiveContextMenu
"Explorer style" (called Immersive by Microsoft) context menu reverse engineered from Windows.

![Dark screenshot](https://github.com/ysc3839/ImmersiveContextMenu/raw/master/dark.png)
![Light screenshot](https://github.com/ysc3839/ImmersiveContextMenu/raw/master/light.png)

Explorer has its own style for context menu, which looks much better than the default style, especially in light mode.\
After some research, I found it's not as simple as the [dark mode menu](https://github.com/ysc3839/win32-darkmode). Explorer uses owner draw to achieve such style.\
This project is a reverse engineered implementation of "Explorer style" context menu.\
Since this project is complex, if you want only dark mode context menus, I suggest using [win32-darkmode](https://github.com/ysc3839/win32-darkmode).\
This project is suitable if you want owner draw something to menu, because turning on owner draw will make the whole menu in a ugly Windows 95 style.

# Bugs and limitations
* No support for menu item bitmaps (`hbmpItem`, `hbmpChecked`, `hbmpUnchecked`). Explorer has this feature.
* No fully support for Per-Monitor DPI, text size may be wrong after moved to another monitor. Explorer has this feature.
* No support for right aligned text (For example: `ABC\tDEF` will make `DEF` right aligned). Explorer has this feature.
* Menu item styles (`MFT_RADIOCHECK`) is not updated dynamically. Explorer DOES NOT has this feature too. Maybe it's because it doesn't reuse menu. It creates a new menu when showing, and destorys it after that. So it's no need to update styles dynamically.
