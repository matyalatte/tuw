# Legacy Renderer on Windows

Static controls (most of the component labels) use Direct2D to draw colored emojis on Windows 8.1 or later.

<img width="393" height="149" alt="new_renderer" src="https://github.com/user-attachments/assets/99c59e7e-6ae9-4fe8-ad74-b689354cfa40" /><br>
<br>
If you don't like the new renderer, you can use `"legacy_renderer": true` to enable the old (GDI-based) renderer.

<img width="392" height="148" alt="legacy_renderer" src="https://github.com/user-attachments/assets/c9b4f15b-f189-4547-9b40-c25e9dfb1de2" />

```json
{
    "legacy_renderer": true,
    "gui": {
        "command": "echo %entry%",
        "components": [
            {
                "type": "text",
                "id": "entry",
                "label": "The legacy renderer uses colorless emojis. 🙂👏"
            }
        ]
    }
}
```

You can also switch the renderer from `Use Legacy Renderer` in the debug menu.

<img width="395" height="149" alt="legacy_renderer_menu" src="https://github.com/user-attachments/assets/5b329927-4ca9-4f36-8b74-581f7556fac2" />
