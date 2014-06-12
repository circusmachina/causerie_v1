{ sourcestreams.inc: Convenience classes that can be used to tokenize source
    code.

  This file is part of parsing.pp, a library of basic and useful parser classes.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ This class is defined for convenience and need not be used, strictly speaking,
  since instances of @link(ASourceScanner) will happily accept any valid
  instance of @link(ATextInputStream) or its descendants.  However, this class
  overrides @link(AStream.Iterator) to make the construction of a source scanner
  for a given source stream more convenient.  It retains a reference to a
  language definition, which is automatically passed to the scanner it creates,
  and it defines a constructor to make wrapping another stream convenient.
}
type ASourceInputStream = class(ATextInputStream)
  protected
    { Refers to the language definition associated with the source stream }
    MyLanguage: AParsedLanguage;

  public
    { Construct a new source input stream that will be parsed using the
      specified language definition and that will wrap the specified target
      stream.

      If @code(takeOwnershipOfTarget) is @true, then the instance will assume
      ownership of @code(ThisTarget) and will free it when it is, itself, freed.

      The language specified by @code(ThisLanguage) will be saved and passed
      along to the instance of @link(ASourceScanner) created when
      @link(ASourceInputStream.Iterator) is called.
    }
    constructor using(const ThisLanguage: AParsedLanguage;
      const ThisTarget: AStream; const takeOwnershipOfTarget: boolean = true);
      virtual;

    { Initializer }
    function init: boolean; override;

    // CanIterate implementation -----------------------------------------------
    { Construct an iterator suitable for iterating over the contents of the
      stream.

      This method overrides the behavior inherited from @link(AStream): it
      constructs and returns an instance of @link(ASourceScanner), passing
      along the value of @link(ASourceInputStream.Language).  The caller is
      responsible for freeing the scanner when it is no longer required.
    }
    function Iterator: AnIterator; override;

    // Properties --------------------------------------------------------------
    { Retrieve the language definition associated with the source stream.

      This value is set when the stream is constructed by a call to
      @link(ASourceInputStream.using) and cannot be changed.
    }
    function Language: AParsedLanguage; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor ASourceInputStream.using(const ThisLanguage: AParsedLanguage;
  const ThisTarget: AStream; const takeOwnershipOfTarget: boolean = true);

begin
  inherited new;

  MyLanguage := ThisLanguage;
  MyTarget := ThisTarget;
  myOwnTarget := takeOwnershipOfTarget;
end;

// Initializer
function ASourceInputStream.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLanguage := nil;
  end;
end;

// CanIterate implementation ---------------------------------------------------
// Construct and return a scanner for the source
function ASourceInputStream.Iterator: AnIterator;

begin
  Result := ASourceScanner.forSource(Self, MyLanguage);
end;

// Properties ------------------------------------------------------------------
// Retrieve the language definition associated with the source
function ASourceInputStream.Language: AParsedLanguage;

begin
  Result := MyLanguage;
end;
{$ENDIF}
