(require 'modCom)
(require 'modControl)

(define server (new RaviControlServer "coucou"))

(define output (new TcpUdpClientServer (send server GetServiceId)))
(send output Create)
(display output)
(display-l "output : " (send output GetTcpPort) " " (send output GetUdpPort) "\n")
(display "ajout output\n")

(send (send (send server GetProperties) c-array "my") = "prop")

 (let ((ioa *NULL*)
       )
 
   (set! ioa (send server AddInOutput "coucou" (send output Cast)  1))
   )

 (display " start server\n")
 (send server StartServer)

 (while #t (send server ProcessMessages))