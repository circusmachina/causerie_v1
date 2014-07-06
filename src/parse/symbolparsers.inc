{ symbolparsers.inc: A class to parse a source stream using a language
    definition and symbol tables.

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
// ASymbolParser ***************************************************************
{ This class represents a parser that processes a source file and enters any
  symbols found (variable names, function names, custom types, etc.) into its
  symbol tables.  This class does not add much to what it inherits from
  @link(AParsedLanguageParser) because it is intended to serve as a template
  from which a custom parsing class may be derived.  It does, however,
  automaticaly construct an instance of @link(ASymbolTableVector) and
  an instance of @link(ASymbolTable) to serve as the global symbol table.  It
  also provides support routines that help to enter new symbols and to look up
  existing ones.
}
type ASymbolParser = class(AParsedLanguageParser)
  protected
    { Refers to the list of symbol tables maintained by the parser }
    MySymbols: ASymbolTableVector;
    { Stores the current scope (the current symbol table in use at the
      current location in the source).
    }
    myCurrentScope: TSymbolScope;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the parser.

      This method frees @link(ASymbolParser.Symbols) and then calls the
      inherited routine.
    }
    destructor destroy; override;

    { Parse the source.

      This method builds on the behavior inherited from
      @link(AParsedLanguageParser.parse).  It calls the inherited routine and,
      if that routine returns zero (0) to indicate all is well, it checks to
      determine whether @link(ASymbolParser.Symbols) is @nil.  If so, it
      constructs an instance of @link(ASymbolTableVector) and then inserts an
      instance of @link(ASymbolTable) into it in order to represent the global
      scope.  This behavior allows a descendant class to construct a different
      class or fill the symbol table vector in a different way.

      @return(A negative value if an error has occurred before any parsing
      was done.  This will most likely occur if @link(AParser.Source),
      @link(AParser.Log), @link(AParsedLanguageParser.Language), or
      @link(ASymbolParser.Symbols) are invalid (@nil) pointers.  A positive
      value indicates the number of non-fatal errors that occurred while parsing.
      A value of zero (0) indicates all is well.)
    }
    function parse: int64; override;

    { Enter a new symbol into the symbol table at the specified scope.

      This method adds @code(ThisSymbol) to the symbol table located at
      @code(thisScope).  If the symbol already exists in the table, it logs a
      syntax error.

      If @code(freeOnFailure) is @true, then this method will automatically
      free @code(ThisSymbol) if it could not be entered into the table.  This
      behavior allows this method to be called with the results of a
      constructor call (e.g., @link(ASymbolFromSource.named)) as the value of
      @code(ThisSymbol).

      @returns(If the insertion was successful, this method returns
      @code(ThisSymbol); if the insertion was not successful, it returns
      @nil.)
    }
    function EnterSymbolInto(const thisScope: TSymbolScope;
      const ThisSymbol: ASymbol;
      const freeOnFailure: boolean = true): ASymbol; virtual;

    { Enter a new symbol into the current symbol table.

      This method adds @code(ThisSymbol) to the symbol table located at
      @link(ASymbolParser.currentScope).  If the symbol already exists in the
      table, it logs a syntax error.

      If @code(freeOnFailure) is @true, then this method will automatically
      free @code(ThisSymbol) if it could not be entered into the table.  This
      behavior allows this method to be called with the results of a
      constructor call (e.g., @link(ASymbolFromSource.named)) as the value of
      @code(ThisSymbol).

      @returns(If the insertion was successful, this method returns
      @code(ThisSymbol); if the insertion was not successful, it returns
      @nil.)
    }
    function EnterSymbol(const ThisSymbol: ASymbol;
      const freeOnFailure: boolean = true): ASymbol; virtual;

    { Enter a new symbol into the global symbol table.

      This method adds @code(ThisSymbol) to the global symbol table.  If the
      symbol already exists in the table, it logs a syntax error.

      If @code(freeOnFailure) is @true, then this method will automatically
      free @code(ThisSymbol) if it could not be entered into the table.  This
      behavior allows this method to be called with the results of a
      constructor call (e.g., @link(ASymbolFromSource.named)) as the value of
      @code(ThisSymbol).

      @returns(If the insertion was successful, this method returns
      @code(ThisSymbol); if the insertion was not successful, it returns
      @nil.)
    }
    function EnterGlobalSymbol(const ThisSymbol: ASymbol;
      const freeOnFailure: boolean = true): ASymbol; virtual;

    { Find a symbol with the given name by searching first in the specified
      scope and then outward.

      This method calls @link(ASymbolTable.SymbolNamed) on the symbol table
      located at @code(thisScope) and returns the result.

      @returns(If the symbol is found in either @code(thisScope) or a larger
      scope, then a reference to it is returned; otherwise, this method returns
      @nil.)
    }
    function NamedSymbolIn(const thisScope: TSymbolScope;
      const thisName: string): ASymbol; virtual;

    { Find a symbol with the given name by searching first in the current
      scope and then outward.

      This method calls @link(ASymbolTable.SymbolNamed) on the symbol table
      located at @link(ASymbolParser.currentScope) and returns the result.

      @returns(If the symbol is found in either the current scope or a larger
      scope, then a reference to it is returned; otherwise, this method returns
      @nil.)
    }
    function SymbolNamed(const thisName: string): ASymbol; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the vector of symbol tables maintained by the
      parser.  The reference returned by this routine should NOT be freed by
      the caller.
    }
    function Symbols: ASymbolTableVector; virtual;

    { Retrieve a reference to the symbol table at the given scope.  The
      reference returned by this routine should NOT be freed by the caller.

      If @code(thisScope) represents a value that is greater than or equal to
      the value of @link(AVector.length ASymbolTableVector.length), then this
      routine will return a reference to the global symbol table.
    }
    function SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;
      virtual;

    { Retrieve a reference to the symbol table that represents the global scope.
      The reference returned by this routine should NOT be freed by the caller.
    }
    function GlobalSymbols: ASymbolTable; virtual;

    { Retrieve a reference to the symbol table that represents the current
      scope.  The reference returned by this routine should NOT be freed by the
      caller.

      This method simply calls @link(ASymbolParser.SymbolTableAt) with the
      current value of @link(ASymbolParser.currentScope) and returns the result.
    }
    function CurrentSymbols: ASymbolTable; virtual;

    { Retrieve the current scope.

      The value returned by this routine represents the symbol table currently
      in use at the current token position within the source being parsed.
    }
    function currentScope: TSymbolScope; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the format of the error message output when there is
    an attempt to define a symbol that has the same name as one which has
    already been defined in the current scope.  This message is output as a
    syntax error by @link(ASymbolParser.EnterSymbolInto).

    The first string placeholder is filled with the name of the symbol, as
    returned by a call to @link(ASymbol.toString).  The second string
    placeholder is filled with the name of the source stream in which the
    symbol was declared, as returned by a call to
    @link(ASymbolFromSource.sourceName).  The integer placeholder is filled
    with the line number at which the symbol was declared, as returned by a call
    to @link(ASymbolFromSource.sourceLine).
  }
  sympErrorDuplicateIdentifier = '%s duplicates symbol that was ' +
    'previously declared in "%s" on line %d';
{$ENDIF}
{$IFDEF ReadImplementation}
// ASymbolParser ***************************************************************
// Initializer
function ASymbolParser.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MySymbols := nil;
    myCurrentScope := -1;
  end;
end;

// Destructor
destructor ASymbolParser.destroy;

begin
  // Free the symbol table collection
  if MySymbols <> nil then
    MySymbols.free;

  inherited destroy;
end;

// Parse the source
function ASymbolParser.parse: int64;

begin
  result := inherited parse;

  if result <> 0 then
    exit;

  // Check to ensure we have a valid symbol table collection
  if MySymbols = nil then
  begin
    result := -1;

    MySymbols := ASymbolTableVector.new;
    if MySymbols = nil then
      exit;

    // Add the global scope to the collection
    MySymbols.pushTable(ASymbolTable.new);

    if MySymbols.length > 0 then
      result := 0;
  end;
end;

// Enter a new symbol into the specified table
function ASymbolParser.EnterSymbolInto(const thisScope: TSymbolScope;
  const ThisSymbol: ASymbol; const freeOnFailure: boolean = true): ASymbol;

var
  // Refers to an existing symbol
  ExistingSymbol: ASymbolFromSource;

begin
  Result := nil;

  // We cannot insert nothing
  if ThisSymbol = nil then
    exit;

  // Check to ensure we do not have a duplicate symbol
  if MySymbols.TableAt(thisScope).hasSymbolNamed(ThisSymbol.name) then
  begin
    // Fetch the existing symbol so it can be used for error output
    ExistingSymbol := ASymbolFromSource(
      MySymbols.TableAt(thisScope).SymbolNamed(ThisSymbol.name)
    );

    syntaxError(sympErrorDuplicateIdentifier, [
      ThisSymbol.toString, ExistingSymbol.sourceName,
      ExistingSymbol.sourceLine
    ]);

    exit;
  end;

  // Ensure we have a good scope reference
  if thisScope > MySymbols.length then
  begin
    if MySymbols.TableAt(thisScope).addSymbol(ThisSymbol, false) then
      Result := ThisSymbol

    else if freeOnFailure then
      ThisSymbol.free;
  end

  else begin
    if MySymbols.TableAt(SYMSCOPE_GLOBAL).addSymbol(ThisSymbol, false) then
      Result := ThisSymbol

    else if freeOnFailure then
      ThisSymbol.free;
  end;
end;

// Enter a new symbol into the current symbol table
function ASymbolParser.EnterSymbol(const ThisSymbol: ASymbol;
  const freeOnFailure: boolean = true): ASymbol;

begin
  Result := Self.EnterSymbolInto(myCurrentScope, ThisSymbol, freeOnFailure);
end;

// Enter a new symbol into the global symbol table
function ASymbolParser.enterGlobalSymbol(const ThisSymbol: ASymbol;
  const freeonFailure: boolean = true): ASymbol;

begin
  Result := Self.EnterSymbolInto(SYMSCOPE_GLOBAL, ThisSymbol, freeOnFailure);
end;

// Find the named symbol in the given scope or a larger scope
function ASymbolParser.NamedSymbolIn(const thisScope: TSymbolScope;
  const thisName: string): ASymbol;

begin
  Result := nil;

  // Ensure we have a good scope reference
  if thisScope < MySymbols.length then
    Result := MySymbols.TableAt(thisScope).SymbolNamed(thisName)

  else
    Result := MySymbols.TableAt(SYMSCOPE_GLOBAL).SymbolNamed(thisName);
end;

// Find the named symbol in the current scope or a larger scope
function ASymbolParser.SymbolNamed(const thisName: string): ASymbol;

begin
  Result := MySymbols.TableAt(myCurrentScope).SymbolNamed(thisName);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the vector of symbol tables used by the parser
function ASymbolParser.Symbols: ASymbolTableVector;

begin
  Result := MySymbols;
end;

// Retrieve a reference to the symbol table that represents the specified scope
function ASymbolParser.SymbolTableAt(const thisScope: TSymbolScope): ASymbolTable;

begin
  // Ensure we have a good scope
  if thisScope < MySymbols.length then
    Result := MySymbols.TableAt(thisScope)

  else
    Result := MySymbols.TableAt(SYMSCOPE_GLOBAL);
end;

// Retrieve a reference to the global symbol table
function ASymbolParser.GlobalSymbols: ASymbolTable;

begin
  Result := MySymbols.TableAt(SYMSCOPE_GLOBAL);
end;

// Retrieve a reference to the symbol table at the current scope
function ASymbolParser.CurrentSymbols: ASymbolTable;

begin
  Result := MySymbols.TableAt(myCurrentScope);
end;

// Retrieve the current scope of the parser
function ASymbolParser.currentScope: TSymbolScope;

begin
  result := myCurrentScope;
end;
{$ENDIF}

