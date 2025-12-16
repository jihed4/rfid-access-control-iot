# API (Google Apps Script)

Base URL (example):
`https://script.google.com/macros/s/<DEPLOYMENT_ID>/exec`

## Log event
**GET**
`?action=log&uid=<UID>&name=<NAME>&status=<GRANTED|DENIED>&door_id=<DOOR>`

- Firmware sends `name` empty. Backend resolves real name from `users` sheet.
- Response: `OK` (text)

## Get users
**GET**
`?action=get_users`

Response JSON:
```json
[
  {"uid":"10 C2 69 59","name":"jihed","role":"admin","active":true}
]
```

## Get logs
**GET**
`?action=get_logs&limit=100`

Response JSON:
```json
[
  {"timestamp":"2025-12-16T09:10:00.000Z","uid":"...","name":"...","status":"GRANTED","door_id":"door1"}
]
```
