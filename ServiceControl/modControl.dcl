;; -*- Scheme -*-

(require 'modCom)
(require 'modPrimaSystem)

(cpp-macro-expand FUNCTION_CALL_TYPE)

(namespace ost (class Mutex)
	       (class Thread)
	       (class XMLStream)
	       (class Event)
	       (class AtomicCounter))

(namespace std (typedef fstream)
		    (class ifstream)
		    (class ofstream)
		    (class istream)
		    (class ostream)
		    (class string)
		    (class-template list)
		    (class-template vector)
		    (class-template complex)
		    )

(class Attribut abstract)

(class ControlServer)
(class RaviControlServer)
(class InOutputAttribut)
(class VariableAttribut)

(class ControlClient)

