Yarvis - Protocol draft
=============

Yarp - yet another remote protocol
-------------

- package type, 1 byte
- package payload, n byte(s)


Package types
------------

### Client Heartbeat

Tell server that you are alive

- type byte: `00000001`

`type`

### Client Subscribe

Tell server that you want to receive events from topic defined in payload

- type byte: `00000010`
- payload:
	- topic byte: from `0x00` to `0xFF`

`type, topic`



### Server Event

Tell client(s) about new data on topics they subscribe to

- type byte `10000000`
- topic byte
- topic payload (length  depends on the topic byte)




Topic Types
------------------------

### beat `0x01`



### fft `0x02`

- fft byte[128]



### intensity `0x03`

- low byte
- mid byte
- high byte
