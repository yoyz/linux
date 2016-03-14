import sys

ERROR_XML_MEMBER_OBJECT_NOT_IMPLEMENTED=100
ERROR_XML_MEMBER_TYPE_NOT_IMPLEMENTED=101
ERROR_XML_MEMBER_GROUP_NOT_IMPLEMENTED=102

ERROR_XML_RULE_NOT_COMPLETED=110           # check the 'rule' class

ERROR_XML_FILE_NOT_FOUND=120
ERROR_XML_FILE_NOT_CORRECT=121

class error_code:

    def __init__(self):
        self.exitcode=0

    def sysexit(self):
        sys.exit(self.exitcode)

    def xml_member_object_not_implemented(self,obj_name):
        print("Warning xml object <"+obj_name+"> not found in template")
        self.exitcode=ERROR_XML_MEMBER_OBJECT_NOT_IMPLEMENTED

    def xml_member_type_not_implemented(self,obj_name):
        print("Warning type <"+obj_name+"> not found in template")
        self.exitcode=ERROR_XML_MEMBER_TYPE_NOT_IMPLEMENTED

    def xml_member_group_not_implemented(self,obj_name):
        print("Warning xml group <"+obj_name+"> not found in template")
        self.exitcode=ERROR_XML_MEMBER_GROUP_NOT_IMPLEMENTED

    def xml_rule_not_completed(self,obj_name):
        print("Error of semantic: <"+obj_name+"> not completed")
        self.exitcode=ERROR_XML_RULE_NOT_COMPLETED

    def xml_file_not_found(self):
        print("Error: file not found ")
        self.exitcode=ERROR_XML_FILE_NOT_FOUND

    def xml_file_not_correct(self):
        print("Error: xml file is not correct")
        self.exitcode=ERROR_XML_FILE_NOT_CORRECT
