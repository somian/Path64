# Copyright 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
#
# Unpublished -- rights reserved under the copyright laws of the
# United States. USE OF A COPYRIGHT NOTICE DOES NOT IMPLY PUBLICATION
# OR DISCLOSURE. THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
# TRADE SECRETS OF PATHSCALE, INC. USE, DISCLOSURE, OR REPRODUCTION IS
# PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF
# PATHSCALE, INC.
#
# U.S. Government Restricted Rights:
# The Software is a "commercial item," as that term is defined at 48
# C.F.R. 2.101 (OCT 1995), consisting of "commercial computer
# software" and "commercial computer software documentation," as such
# terms are used in 48 C.F.R. 12.212 (SEPT 1995).  Consistent with 48
# C.F.R. 12.212 and 48 C.F.R. 227-7202-1 through 227-7202-4 (JUNE
# 1995), all U.S. Government End Users acquire the Software with only
# those rights set forth in the accompanying license
# agreement.
#
# PathScale, Inc. 2071 Stierlin Court, Suite 200; Mountain View CA 94043.

'''XML parser that builds a tree and lets you navigate it '''

__author__ = "David Allison <dallison@pathscale.com>"

import sys

class Attribute:
    def __init__ (self, name, value):
        self.name = name
        self.value = value

    def doprint(self, file):
        str = " " + self.name + "=\"" + self.value + "\""
        file.write (str)

class Element:
    def __init__ (self, name):
        self.name = name
        self.attributes = []
        self.body = ""
        self.children = []
        self.isempty = False

    def dump (self, file, indent=0):
        file.write (" " * indent)
        file.write ("<" + self.name)

        for attr in self.attributes:
            attr.doprint (file)

        if self.isempty:
            file.write ("/>\n")
            return

        file.write (">\n")
        if self.body != "":
            file.write (" " * indent)
            file.write (self.body)

        for child in self.children:
            child.dump (file, indent+4)

        file.write (" " * indent)

        file.write ("</" + self.name + ">\n")

    def doprint (self, file, indent=0):
        if self.name.startswith("XML."):
            file.write (body)
            for child in self.children:
                child.doprint (file, indent+4)
        else:
            file.write (" " * indent)
            file.write ("<" + self.name)

            for attr in self.attributes:
                attr.doprint (file)

            if self.isempty:
                file.write ("/>\n")
                return

            file.write (">\n")
            if self.body != "":
                file.write (" " * indent)
                file.write (self.body)

            for child in self.children:
                child.doprint (file, indent+4)

            file.write (" " * indent)
            file.write ("</" + self.name + ">\n")

    def addAttribute (self, name, value):
        attr = Attribute (name, value)
        self.attributes.append (attr)


    def getBody (self):
        return self.body

    def setBody (self, body):
        self.body = body

    def setEmpty (self):
        self.isempty = True

    def getAttribute (self, n):
        for attr in self.attributes:
            if (attr.name == n):
                return attr.value
        raise Exception, "No such attribute"

    def attributePresent (self, n):
        for attr in self.attributes:
            if (attr.name == n):
                return True
        return False

    def addBody (self, s):
        trimmed = s.strip()
        if trimmed == "":
            return
        bodyelement = Element ("XML.Text")
        bodyelement.setBody (s)
        self.children.append (bodyelement)

    def addChild (self, elem):
        self.children.append (elem)

    def find (self, nm):
        if nm == self.name:
            return self
        for child in self.children:
            r = child.find (nm)
            if (r != None):
                return r
        return None

    def findAttribute (self, nm):
        for attr in self.attributes:
            if attr.name == nm:
                return attr
        return None

def parseStream (file):
    def readToElement():
        eof = False
        result = ""
        wasElement = False
        while not eof:
            ch = file.read(1)
            if ch == "":
               eof = True
               break
            if ch == "<":
               wasElement = True
               break
            result += ch
        return wasElement, result, eof

    def skipSpaces (lastchar = "\0"):
        eof = False
        if lastchar != "\0" and not lastchar.isspace():
            return lastchar, False
        while not eof:
            ch = file.read (1)
            if ch == "":
                eof = True
                break
            if not ch.isspace() or ch == "\n":
                lastchar = ch
                break
        return lastchar, eof

    def readName (lastchar):
        eof = False
        name = ""
        terminators = (">", "=", "/")
        ch, eof = skipSpaces (lastchar)
        name += ch
        while not eof:
            ch = file.read (1)
            if ch == "":
               eof = True
               break
            if ch.isspace() or ch in terminators:
                lastchar = ch
                break
            name += ch
        lastchar, eof = skipSpaces(lastchar)
        name.strip()
        return name, lastchar, eof

    # return tuple of value, lastchar
    def readAttributeValue (lastchar):
        eof = False
        value = ""
        ch, eof = skipSpaces()
        if ch == '"':
            while not eof:
                ch = file.read(1)
                if ch == "":
                    eof = True
                    break
                if ch == '"':
                    lastchar = file.read(1)
                    break
                value += ch
        elif ch == ">":
            raise Exception, "Missing attribute value"
        else:
            value += ch
            while not eof:
                ch = file.read(1)
                if ch == "":
                    eof = True
                    break
                if ch.isspace() or ch == ">":
                    lastchar = ch
                    break
                value += ch
        lastchar, eof = skipSpaces (lastchar)
        return value, lastchar, eof

    eof = False

    stack = []
    stack.append (Element ("XML.top"))

    foundElement, initial, eof = readToElement()

    stack[-1].addBody (initial)

    while not eof:
        if foundElement:
            termchar = '\0'
            name, termchar, eof = readName (termchar)
            if name:
                if name.startswith('!--'):
                    while not eof:
                        ch = file.read(1)
                        if ch == "" or ch == '>':
                            break
                elif name.startswith('/'):
                    name = name[1:]
                    top = stack[-1]
                    if top.name == name:
                        del stack[-1]
                    else:
                        raise Exception, "Element " + name + " is not top of stack"
                else:
                    element = Element (name)
                    stack[-1].addChild (element)

                    empty = False
                    while termchar != '>':
                        attrname, termchar, eof = readName (termchar)
                        if attrname == '/' and termchar == '>':
                            empty = True
                            element.setEmpty()
                        elif termchar == '=':
                            attrvalue, termchar, eof = readAttributeValue (termchar)
                            element.addAttribute (attrname, attrvalue)
                        else:
                            element.addAttribute (attrname, "")
                    if not empty:
                        stack.append (element)

        foundElement, body, eof = readToElement()
        if body != "":
            stack[-1].addBody (body)

    return stack[-1]


