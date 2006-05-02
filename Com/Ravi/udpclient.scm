(require 'modCom)

(define client *NULL*)

(define msgManager (new MsgManager))


;;hyperion : 194.199.23.175
(define (client-init)
  (set! client (new UdpClient "hyperion" 5000))
  (send msgManager LinkToClient client)
  ;;(send client setCallBackOnRecv MsgManager::pushMessage msgManager *NULL*)
)

(define msg "titi")

(define (client-send)
  (send client SendToServer (string-length msg) msg)
)

