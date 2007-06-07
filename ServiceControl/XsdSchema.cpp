/* @file XsdSchema.cpp
 * @ingroup ServiceControl
 * @brief Body of the Xsd Schema Strings definition. Automatically generated using GenerateXsdSupportFiles script.
 * @date 2004-2006
 */
#include <ServiceControl/XsdSchema.h>

using namespace Omiscid;

// Specific usage of string because of limitation of for constant string.
// This file is automatically generated using GenerateXsdSupportFiles.pl script.
const SimpleString Omiscid::ControlAnswerXsdSchema = ""
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" elementFormDefault=\"qualified\" version=\"1.0\">\n"
"  <xs:simpleType name=\"IDType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9]\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:simpleType name=\"NameType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-zA-Z0-9_]*\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:simpleType name=\"VariableNameType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-zA-Z0-9_ ]*\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:simpleType name=\"AccessType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:enumeration value=\"constant\" />\n"
"      <xs:enumeration value=\"read\" />\n"
"      <xs:enumeration value=\"readWrite\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:complexType name=\"PeersType\">\n"
"    <xs:sequence>\n"
"      <xs:element name=\"peer\" type=\"IDType\" minOccurs=\"0\" maxOccurs=\"unbounded\" />\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"  <xs:complexType name=\"CA_InOutputType\">\n"
"    <xs:all>\n"
"      <xs:element name=\"tcp\" type=\"xs:nonNegativeInteger\" minOccurs=\"0\" />\n"
"      <xs:element name=\"udp\" type=\"xs:nonNegativeInteger\" minOccurs=\"0\" />\n"
"      <xs:element name=\"description\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"formatDescription\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"peers\" type=\"PeersType\" minOccurs=\"0\" />\n"
"      <xs:element name=\"peerId\" type=\"IDType\" minOccurs=\"0\" />\n"
"      <xs:element name=\"require\" type=\"xs:positiveInteger\" minOccurs=\"0\" />\n"
"    </xs:all>\n"
"    <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"  </xs:complexType>\n"
"  <xs:complexType name=\"CA_VariableType\">\n"
"    <xs:all>\n"
"      <xs:element name=\"access\" type=\"AccessType\" minOccurs=\"0\" />\n"
"      <xs:element name=\"value\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"default\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"type\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"description\" type=\"xs:string\" minOccurs=\"0\" />\n"
"      <xs:element name=\"formatDescription\" type=\"xs:string\" minOccurs=\"0\" />\n"
"    </xs:all>\n"
"    <xs:attribute name=\"name\" type=\"VariableNameType\" use=\"required\" />\n"
"  </xs:complexType>\n"
"  <xs:simpleType name=\"CA_LockResultType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:enumeration value=\"ok\" />\n"
"      <xs:enumeration value=\"failed\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:complexType name=\"CA_LockType\">\n"
"    <xs:attribute name=\"result\" type=\"CA_LockResultType\"  use=\"required\"/>\n"
"    <xs:attribute name=\"peer\" type=\"IDType\"  use=\"required\"/>\n"
"  </xs:complexType>\n"
"  <xs:element name=\"controlAnswer\">\n"
"    <xs:complexType>\n"
"      <xs:choice minOccurs=\"0\" maxOccurs=\"unbounded\">\n"
"        <xs:element name=\"input\" type=\"CA_InOutputType\" />\n"
"        <xs:element name=\"output\" type=\"CA_InOutputType\" />\n"
"        <xs:element name=\"inoutput\" type=\"CA_InOutputType\" />\n"
"        <xs:element name=\"variable\" type=\"CA_VariableType\" />\n"
"        <xs:element name=\"lock\" type=\"CA_LockType\" />\n"
"        <xs:element name=\"unlock\" type=\"CA_LockType\" />\n"
"      </xs:choice>\n"
"      <xs:attribute name=\"id\" type=\"IDType\" use=\"required\" />\n"
"    </xs:complexType>\n"
"  </xs:element>\n"
"  <xs:element name=\"controlEvent\">\n"
"    <xs:complexType>\n"
"      <xs:all>\n"
"        <xs:element name=\"variable\" type=\"CA_VariableType\" minOccurs=\"0\"/>\n"
"      </xs:all>\n"
"    </xs:complexType>\n"
"  </xs:element>\n"
"</xs:schema>\n"
;

const SimpleString Omiscid::ControlQueryXsdSchema = ""
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" elementFormDefault=\"qualified\" version=\"1.0\">\n"
"  <xs:simpleType name=\"IDType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9][a-fA-F0-9]\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:simpleType name=\"NameType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-zA-Z0-9_]*\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:simpleType name=\"VariableNameType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:pattern value=\"[a-zA-Z0-9_ ]*\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:element name=\"controlQuery\">\n"
"    <xs:complexType>\n"
"      <xs:choice minOccurs=\"0\" maxOccurs=\"unbounded\">\n"
"        <xs:element name=\"fullDescription\">\n"
"          <xs:complexType/>\n"
"        </xs:element>\n"
"        <xs:element name=\"input\">\n"
"          <xs:complexType>\n"
"            <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"output\">\n"
"          <xs:complexType>\n"
"            <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"inoutput\">\n"
"          <xs:complexType>\n"
"            <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"variable\">\n"
"          <xs:complexType>\n"
"            <xs:sequence>\n"
"              <xs:element name=\"value\" type=\"xs:string\" minOccurs=\"0\" maxOccurs=\"1\" />\n"
"            </xs:sequence>\n"
"            <xs:attribute name=\"name\" type=\"VariableNameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"connect\">\n"
"          <xs:complexType>\n"
"            <xs:sequence>\n"
"              <xs:element name=\"host\" type=\"xs:string\" minOccurs=\"1\" maxOccurs=\"1\" />\n"
"              <xs:choice minOccurs=\"1\" maxOccurs=\"1\">\n"
"                <xs:element name=\"tcp\" type=\"xs:positiveInteger\" />\n"
"                <xs:element name=\"udp\" type=\"xs:positiveInteger\" />\n"
"              </xs:choice>\n"
"            </xs:sequence>\n"
"            <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"disconnect\">\n"
"          <xs:complexType>\n"
"            <xs:sequence>\n"
"              <xs:element name=\"peer\" type=\"IDType\" minOccurs=\"0\" maxOccurs=\"unbounded\" />\n"
"            </xs:sequence>\n"
"            <xs:attribute name=\"name\" type=\"NameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"subscribe\">\n"
"          <xs:complexType>\n"
"            <xs:attribute name=\"name\" type=\"VariableNameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"unsubscribe\">\n"
"          <xs:complexType>\n"
"            <xs:attribute name=\"name\" type=\"VariableNameType\" use=\"required\" />\n"
"          </xs:complexType>\n"
"        </xs:element>\n"
"        <xs:element name=\"lock\">\n"
"          <xs:complexType/>\n"
"        </xs:element>\n"
"        <xs:element name=\"unlock\">\n"
"          <xs:complexType/>\n"
"        </xs:element>\n"
"      </xs:choice>\n"
"      <xs:attribute name=\"id\" type=\"IDType\" use=\"required\" />\n"
"    </xs:complexType>\n"
"  </xs:element>\n"
"  <xs:simpleType name=\"AccessType\">\n"
"    <xs:restriction base=\"xs:string\">\n"
"      <xs:enumeration value=\"constant\" />\n"
"      <xs:enumeration value=\"read\" />\n"
"      <xs:enumeration value=\"readWrite\" />\n"
"    </xs:restriction>\n"
"  </xs:simpleType>\n"
"  <xs:complexType name=\"PeersType\">\n"
"    <xs:sequence>\n"
"      <xs:element name=\"peer\" type=\"IDType\" minOccurs=\"0\" maxOccurs=\"unbounded\" />\n"
"    </xs:sequence>\n"
"  </xs:complexType>\n"
"</xs:schema>\n"
;

const SimpleString Omiscid::ServiceXsdSchema = ""
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<schema xmlns=\"http://www.w3.org/2001/XMLSchema\" targetNamespace=\"http://www-prima.inrialpes.fr/schemas/omiscid/service.xsd\" xmlns:omiscid=\"http://www-prima.inrialpes.fr/schemas/omiscid/service.xsd\">\n"
"    <element name=\"service\">\n"
"        <complexType>\n"
"            <choice maxOccurs=\"unbounded\" minOccurs=\"0\">\n"
"                <element ref=\"omiscid:variable\" />\n"
"                <element ref=\"omiscid:input\" />\n"
"                <element ref=\"omiscid:output\" />\n"
"                <element ref=\"omiscid:inoutput\" />\n"
"            </choice>\n"
"            <attribute name=\"name\" type=\"string\" use=\"required\" />\n"
"            <attribute name=\"docURL\" type=\"string\" use=\"optional\" />\n"
"            <attribute name=\"class\" type=\"string\" use=\"optional\" />\n"
"        </complexType>\n"
"    </element>\n"
"    <element name=\"variable\">\n"
"        <complexType>\n"
"            <all>\n"
"                <element name=\"description\" type=\"string\" minOccurs=\"0\" />\n"
"                <element name=\"formatDescription\" type=\"string\" minOccurs=\"0\" />\n"
"                <element name=\"value\" type=\"string\" minOccurs=\"0\" />\n"
"                <element name=\"default\" type=\"string\" minOccurs=\"0\" />\n"
"                <element name=\"type\" type=\"string\" minOccurs=\"0\" />\n"
"                <element name=\"access\" type=\"omiscid:accessType\" minOccurs=\"1\" maxOccurs=\"1\"/>\n"
"            </all>\n"
"            <attribute name=\"name\" type=\"string\" use=\"required\"></attribute>\n"
"        </complexType>\n"
"    </element>\n"
"    <simpleType name=\"accessType\">\n"
"        <restriction base=\"string\">\n"
"            <enumeration value=\"constant\"></enumeration>\n"
"            <enumeration value=\"read\"></enumeration>\n"
"            <enumeration value=\"readWrite\"></enumeration>\n"
"        </restriction>\n"
"    </simpleType>\n"
"    <complexType name=\"connectorType\">\n"
"        <all>\n"
"            <element name=\"description\" type=\"string\" minOccurs=\"0\" />\n"
"            <element name=\"formatDescription\" type=\"string\" minOccurs=\"0\" />\n"
"            <element name=\"schemaURL\" type=\"string\" minOccurs=\"0\" />\n"
"            <element name=\"messageExample\" type=\"string\" minOccurs=\"0\" />\n"
"        </all>\n"
"        <attribute name=\"name\" type=\"string\" use=\"required\"></attribute>\n"
"    </complexType>\n"
"    <element name=\"input\" type=\"omiscid:connectorType\"></element>\n"
"    <element name=\"output\" type=\"omiscid:connectorType\"></element>\n"
"    <element name=\"inoutput\" type=\"omiscid:connectorType\"></element>\n"
"</schema>\n"
;
































































