# MovieTracker — Manual Test Plan

The automated QTest suite covers logic, storage, parsing and signal wiring. It
**cannot** verify what only a human can judge: visual layout, live theming,
animations, real OMDb network calls, native file/color dialogs, the clipboard,
the browser hand-off, audio, and window behaviour. This checklist covers every
action a user can take, derived from the source. Run it before tagging a
release.

**Prerequisites**
- A valid OMDb API key (Settings > API Key) for anything involving search,
  adding titles, or season updates.
- An internet connection for those same flows.
- At least a few titles in the library (some watched, some not, some ranked) to
  exercise sorting/filtering/ranking.

---

## 1. App lifecycle & window

- [ ] **1.1** Launch the app — main window opens at the last-saved size (1200×800 first run); library shown on the Movies tab
- [ ] **1.2** Resize the window — card grid reflows to fit the new width after a brief pause; columns/spacing recompute, no overlap
- [ ] **1.3** Resize, close, relaunch — window reopens at the size it was closed at
- [ ] **1.4** Press Ctrl+W (Cmd+W on macOS) — app quits
- [ ] **1.5** Launch with a key set and a TV series due for a season check — full-screen "Looking for new TV show seasons…" overlay with a spinner appears; menu bar disabled during it, re-enabled after

## 2. Top bar (library mode)

- [ ] **2.1** Click the **Movies** / **TV shows** tabs — library switches to that type; active tab highlighted
- [ ] **2.2** Click the already-active tab — nothing changes (no flicker / re-query)
- [ ] **2.3** Click the **sort** icon — menu opens with A–Z, Release, Watch date, Rank
- [ ] **2.4** Pick each sort option — library reorders accordingly (Rank also hides unranked titles and shows a "#n" prefix)
- [ ] **2.5** Click the **rank** icon with watched-but-unranked titles present — enters the ranking view
- [ ] **2.6** Click **rank** when everything is ranked / nothing is watched — a disabled "All titles are ranked" menu appears; ranking does not start
- [ ] **2.7** Click the **+** icon — enters add mode (top bar replaced by the add bar)
- [ ] **2.8** Click the **bell** icon — notifications panel pops up under the bell

## 3. Library filter / zoom / search bar

- [ ] **3.1** Click **All** / **To watch** — library filters; active button highlighted; clicking the active one does nothing
- [ ] **3.2** Click **zoom in** / **zoom out** — cards grow/shrink (clamped between min and max sizes); size persists across restart
- [ ] **3.3** Hold a zoom button — auto-repeats while held
- [ ] **3.4** Click the **search** icon — search field appears and is focused; search icon replaced by a close (✕)
- [ ] **3.5** Type a query and press Enter — library filters to fuzzy matches
- [ ] **3.6** Clear the search text — library returns to showing all titles
- [ ] **3.7** Press Escape / click the close button — search closes, field clears, full library restored

## 4. Add mode & search results

- [ ] **4.1** Enter add mode — search field is auto-focused
- [ ] **4.2** Type a title and press Enter — spinner shows, then OMDb results appear (poster, title, year, plot)
- [ ] **4.3** Press Enter with empty / whitespace-only text — no search is run
- [ ] **4.4** Search a nonsense string — "No movies found" graphic is shown
- [ ] **4.5** Search with an invalid/empty API key — error card appears with the API-key message
- [ ] **4.6** Click **Add** on a result — row shows a spinner, then the button switches to the "added" state; title is in the library
- [ ] **4.7** Click the **added** button — title is removed from the library; button reverts to Add
- [ ] **4.8** Click the close button / press Escape — returns to the library

## 5. Title card (library grid)

- [ ] **5.1** Hover a card — action buttons fade in (delete; watch toggle; unrank if ranked; upload-poster if artwork missing)
- [ ] **5.2** Move the cursor off the card — all action buttons hide again
- [ ] **5.3** Click the card body — opens the title detail view
- [ ] **5.4** Click **To watch** — title marked watched; button swaps to **Watched**
- [ ] **5.5** Click **Watched** — title marked unwatched; button swaps back
- [ ] **5.6** Click the **delete** (trash) button — title removed from the library immediately
- [ ] **5.7** Click the **upload poster** button (only shown when artwork is missing) — file picker opens; choosing an image sets it as the poster and the button disappears
- [ ] **5.8** Click the **unrank** button (only shown when ranked) — rank is cleared; other ranks shift up

## 6. Title detail view

- [ ] **6.1** Open a title — poster, title, year/type, metadata rows, plot, and "Watch on" section render
- [ ] **6.2** Click **back** — returns to the library
- [ ] **6.3** Toggle **To watch / Watched** — watched state and the "Last watched" row update live
- [ ] **6.4** Click **delete** — title removed and view returns to the library
- [ ] **6.5** Click a streaming-platform button — opens the browser to that service's search for this title
- [ ] **6.6** Click **Try all** — opens every configured platform at once
- [ ] **6.7** Open a title with no platforms configured — "No custom streaming platforms…" hint is shown instead of buttons

## 7. Ranking view

- [ ] **7.1** Start ranking — two title cards side by side, "Which do you prefer?", progress "Ranking Movies: x / y"
- [ ] **7.2** Click the left or right card — choice recorded; next comparison shown (binary search — few comparisons per title)
- [ ] **7.3** Finish all movies — phase switches to "Ranking TV Shows" if any are pending
- [ ] **7.4** Finish everything — ranking view closes, returns to library
- [ ] **7.5** Click **exit** mid-session — ranking closes; titles ranked so far keep their positions
- [ ] **7.6** Resize the window during ranking — cards rescale to fit

## 8. Notifications

- [ ] **8.1** A tracked series gains a new season (via season update) — a red dot appears on the bell and a notification sound plays
- [ ] **8.2** Open the notifications panel — new-season alerts listed with poster + title
- [ ] **8.3** Click a notification — navigates to that title's detail page; panel closes
- [ ] **8.4** Open the panel (with notifications) — red dot clears after opening
- [ ] **8.5** No notifications — panel shows "No notifications"; no red dot

## 9. Menu bar

- [ ] **9.1** Library > Settings — Settings dialog opens (modal)
- [ ] **9.2** Library > Export library — save dialog (.zip); a backup archive is written; failure shows a warning
- [ ] **9.3** Library > Import library — open dialog (.zip), then "overwrite" confirmation; on OK the library is validated and replaced
- [ ] **9.4** Import: cancel the confirmation — nothing changes
- [ ] **9.5** Import an invalid / tampered zip — "Invalid file" warning with a reason; library unchanged
- [ ] **9.6** Export then Import the same file — library round-trips identically (titles, posters, platforms, key)
- [ ] **9.7** Help > How to use — Help dialog opens

## 10. Settings dialog

- [ ] **10.1** Click each tab (Appearance / API Key / Platforms / Rankings) — the matching section is shown
- [ ] **10.2** Appearance > toggle Light / Dark — theme changes live across the whole app; no restart, no leftover colors
- [ ] **10.3** Appearance > Change color — color picker opens; chosen accent applies live and is stored per-theme
- [ ] **10.4** Appearance > Reset — accent reverts to the theme default; the Reset button hides when already default
- [ ] **10.5** API Key field — pre-filled with the saved key
- [ ] **10.6** API Key > type a key, click Add Key — button shows "Adding…", disables, then re-enables; key is saved
- [ ] **10.7** API Key > empty / whitespace key — Add Key stays disabled; nothing saved
- [ ] **10.8** Platforms list — each platform shows logo + name + a delete button
- [ ] **10.9** Platforms > delete a platform — it disappears from the list and from title detail "Watch on"
- [ ] **10.10** Platforms > Add (with fewer than 10 platforms) — add-platform dialog opens; the Add button is hidden once 10 exist
- [ ] **10.11** Rankings > Reset Movies / Reset TV Shows — confirmation dialog; on confirm, that type's ranks are cleared
- [ ] **10.12** Rankings > cancel the confirmation — ranks unchanged

## 11. Add streaming platform dialog

- [ ] **11.1** Type a Name and Search URL — **Add** enables only when both are filled
- [ ] **11.2** Type a name that already exists — "already exists" error shows; Add stays disabled
- [ ] **11.3** Click **Copy "movietracker"** — clipboard contains `movietracker`; button shows "Copied!" for ~1.5 s
- [ ] **11.4** Click the logo **browse** button — file picker opens; chosen path appears in the (read-only) field
- [ ] **11.5** Click **Add** — platform is added (logo copied into storage); Settings list refreshes
- [ ] **11.6** Click **Cancel** — dialog closes; nothing added

## 12. Help dialog

- [ ] **12.1** Open Help — all help topics are listed
- [ ] **12.2** Type in the search box — list filters to matching topics (case-insensitive)
- [ ] **12.3** Clear the search box — all topics shown again

## 13. Theming regression sweep

After changing theme or accent, click through every screen and confirm colors
update with no leftovers:

- [ ] **13.1** Top bar + tab buttons
- [ ] **13.2** Library grid + title cards
- [ ] **13.3** Add bar + search results
- [ ] **13.4** Title detail (incl. streaming buttons)
- [ ] **13.5** Ranking view
- [ ] **13.6** Settings + Add-platform dialog
- [ ] **13.7** Help dialog
- [ ] **13.8** Notifications panel
