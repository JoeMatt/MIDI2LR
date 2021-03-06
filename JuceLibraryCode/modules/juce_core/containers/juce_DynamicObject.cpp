/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2016 - ROLI Ltd.

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

   -----------------------------------------------------------------------------

   To release a closed-source product which uses other parts of JUCE not
   licensed under the ISC terms, commercial licenses are available: visit
   www.juce.com for more information.

  ==============================================================================
*/

DynamicObject::DynamicObject()
{
}

DynamicObject::DynamicObject (const DynamicObject& other)
   : ReferenceCountedObject(), properties (other.properties)
{
}

DynamicObject::~DynamicObject()
{
}

bool DynamicObject::hasProperty (const Identifier& propertyName) const
{
    const var* const v = properties.getVarPointer (propertyName);
    return v != nullptr && ! v->isMethod();
}

const var& DynamicObject::getProperty (const Identifier& propertyName) const
{
    return properties [propertyName];
}

void DynamicObject::setProperty (const Identifier& propertyName, const var& newValue)
{
    properties.set (propertyName, newValue);
}

void DynamicObject::removeProperty (const Identifier& propertyName)
{
    properties.remove (propertyName);
}

bool DynamicObject::hasMethod (const Identifier& methodName) const
{
    return getProperty (methodName).isMethod();
}

var DynamicObject::invokeMethod (Identifier method, const var::NativeFunctionArgs& args)
{
    if (var::NativeFunction function = properties [method].getNativeFunction())
        return function (args);

    return var();
}

void DynamicObject::setMethod (Identifier name, var::NativeFunction function)
{
    properties.set (name, var (function));
}

void DynamicObject::clear()
{
    properties.clear();
}

void DynamicObject::cloneAllProperties()
{
    for (int i = properties.size(); --i >= 0;)
        if (var* v = properties.getVarPointerAt (i))
            *v = v->clone();
}

DynamicObject::Ptr DynamicObject::clone()
{
    Ptr d (new DynamicObject (*this));
    d->cloneAllProperties();
    return d;
}

void DynamicObject::writeAsJSON (OutputStream& out, const int indentLevel, const bool allOnOneLine)
{
    out << '{';
    if (! allOnOneLine)
        out << newLine;

    const int numValues = properties.size();

    for (int i = 0; i < numValues; ++i)
    {
        if (! allOnOneLine)
            JSONFormatter::writeSpaces (out, indentLevel + JSONFormatter::indentSize);

        out << '"';
        JSONFormatter::writeString (out, properties.getName (i));
        out << "\": ";
        JSONFormatter::write (out, properties.getValueAt (i), indentLevel + JSONFormatter::indentSize, allOnOneLine);

        if (i < numValues - 1)
        {
            if (allOnOneLine)
                out << ", ";
            else
                out << ',' << newLine;
        }
        else if (! allOnOneLine)
            out << newLine;
    }

    if (! allOnOneLine)
        JSONFormatter::writeSpaces (out, indentLevel);

    out << '}';
}
