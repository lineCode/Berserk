cmake_minimum_required(VERSION 3.9)
project(BerserkXMLParser)

# XML Parser files

set(SOURCE_LIB

        Private/XMLNode.cpp
        Private/XMLDocument.cpp
        Private/XMLAttribute.cpp
        Private/XMLUtility.cpp

        Public/XMLNode.h
        Public/XMLDocument.h
        Public/XMLAttribute.h
        Public/XMLUtility.h

        )

# Create static XML Parser Subsystem library

add_library(BerserkXMLParser STATIC ${SOURCE_LIB})