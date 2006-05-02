(require 'modCom)

(define server (new TcpServer 5000))

(define msgManager (new MsgManager))
(send msgManager LinkToServer server)

(define msg "toto")

(define (server-send)
  (send server SendToAllClients (string-length msg) msg)
)

