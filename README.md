# gui – GTK-Widget-Bibliothek mit App-Hooks

Wiederverwendbare GTK-UI-Bibliothek in C: Der Ablauf (GTK-Signale,
Event-Dispatcher, Rendering-Pipeline) liegt als Skelett in der Bibliothek,
die Anwendung implementiert nur noch die überschreibbaren Weak-Callbacks
(Muster aus `api`). Ergebnis: C++-artige „virtuelle" Hooks ohne OOP.

## Umfang

- Widgets/Komponenten: `gui_application`, `gui_main_window`, `gui_gl`
  (GLArea/OpenGL), `gui_drawing_area` (Cairo), `gui_frame`, `gui_box`,
  `gui_text`, `gui_button`, `gui_notebook`.
- Zentrales Event-Modell `struct gui_event` + `gui_event_type_t`
  (Startup/Activate/Shutdown, Key, Draw, Click/Toggle, GL-Render/Realize).
- Alle Widgets werden intern registriert
  (`_gui_add_widget_to_internal_list`) und Core-referenziert
  (`_gui_get_core`) – `protected`-Interna der Bibliothek.

## Callback-Muster (via `api`)

Die Anwendung definiert die weak-Defaults der Bibliothek stark:

```c
/* libraries/gui: weak-Default per callback_declaration */
callback_declaration(bool, gui_application(gui_event_type_t event, gui_application_t core));

/* Anwendung: starke Definition überschreibt das weak-Symbol */
callback bool gui_application(gui_event_type_t event, gui_application_t core) { /* ... */ return true; }
```

## Nutzung

```c
#include <gui/gui.h>

callback bool gui_application(gui_event_type_t event, gui_application_t core) { /* ... */ }
callback void gui_main_window(gui_main_window_t core, gui_event_t e) { /* ... */ }
callback void gui_button(gui_button_t core, gui_event_t e) { /* ... */ }

int main(int argc, char** argv)
{
    return gui_application_run("app", argc, argv, NULL);
}
```

## Abhängigkeiten

- `api` (Callback-/Sichtbarkeits-Makros), `threading`, `string`, `logging`
- System: GTK3 (`gtk+-3.0`), OpenGL/epoxy

## Build

```bash
cmake -S . -B build
cmake --build build
```

In ein Projekt einbinden: `add_subdirectory(../../libraries/gui …)`,
Einbindung des Headers über den Include-Pfad `<gui/gui.h>`.