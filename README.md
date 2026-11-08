# QtMessenger

A desktop messenger client written in **C++17 with Qt Widgets and Qt Network**. It supports
sign-up, login, one-to-one chats, groups, and channels, and keeps an offline copy of every
conversation on disk.

> **Status: archived (2023).** The backend this client was built for
> (`api.barafardayebehtar.ml:8080`) is no longer online, so the app cannot log in or sync
> any more. The code is kept as a reference for the client-side work.

## Background

I was hired to build **only the client application**. The server, its API, and its
request/reply format were designed and run by someone else and given to me as a fixed spec
(see [`docs/Server All request and reply format.pdf`](docs/)). Some of the API's design
choices are therefore not mine, and the client had to work around them:

- **No WebSocket or push channel.** A chat backend would normally push new messages. This one
  only answers plain HTTP requests, so the client **polls**: background threads ask for new
  contacts and messages every 5 seconds.
- **Every call is an HTTP `GET`**, including login, sign-up, and sending a message, and all
  parameters (credentials, token, message body) travel in the query string.
- **Lists come back as numbered keys** (`"block 0"`, `"block 1"`, …) instead of JSON arrays,
  and status codes come back as strings (`"code": "200"`).

## Features

- **Accounts:** sign up (username, password, optional first and last name), log in, log out.
- **Auto-login:** the session is saved on login, so the next start goes straight to the chat window.
- **Three conversation types:** direct messages, groups, and channels, all through the same chat UI.
- **Create and join** groups and channels from the menu.
- **Background sync:** the contact list and every conversation refresh every 5 seconds
  without blocking the UI.
- **Offline cache:** every conversation is stored under `data/`, loaded at startup, and synced
  incrementally. The client sends the cached file's last-modified time, so it only requests
  newer messages.
- **Readable errors:** server status codes (200, 204, 401, 404, 500) become messages shown in dialogs.

## Architecture

```
main.cpp
└── Client  (app controller: one Qt slot per API call, plus its reply callback)
    ├── ServerInterface   builds request URLs, parses JSON replies, maps status codes
    ├── LoaderThread      QThread that emits a "time to load" signal every 5 s
    ├── forms/            Qt Designer UIs
    │   ├── Form                  base window + shared popup/message helpers
    │   ├── AuthenticationForm    sign-up / login
    │   └── ChatForm              contact list, chat view, send box, group/channel menu
    └── chat/             domain model
        ├── Contact       polymorphic base (name, type, last-modified time, file path)
        ├── User          also the signed-in account: holds every chat, saves them to disk
        ├── Group
        └── Channel
```

- **Networking:** every request goes through `QNetworkAccessManager`. Replies arrive
  asynchronously, and each one is connected to its own callback slot.
- **Threading:** `LoaderThread` never touches the UI. It only emits signals, and Qt's
  queued connections run the matching `Client` slot on the GUI thread. That is the safe
  way to trigger UI updates from a worker thread.
- **Polymorphism:** `Contact::type()` and `strType()` choose the right endpoint (for example
  `sendmessageuser`, `sendmessagegroup`, `sendmessagechannel`) and the right cache folder.
  The send and load code is written once for all three conversation types.

## Build

Requirements: CMake 3.5+, a C++17 compiler, and Qt 5 or Qt 6 with the Widgets and Network
modules.

```bash
cmake -S . -B build
cmake --build build
./build/Messenger
```

Or open `CMakeLists.txt` in Qt Creator.

The app creates a `data/` folder next to where it runs. Logging out deletes that folder.

## Known limitations

This is a 2023 project kept as-is. A production version would need these changes:

- Saved credentials sit in a plain-text file (`data/me.cred`). They should go in the OS
  keychain, and only a token should be stored.
- Polling would be replaced by a push channel (WebSocket or server-sent events) if the server
  offered one.
- The server address is hard-coded in `serverinterface.cpp` and should be configurable.

## Author

Pedram Makanparast (PaYa): [github.com/pya-h](https://github.com/pya-h)
