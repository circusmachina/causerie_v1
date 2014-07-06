{ symbolcollections.inc - Classes to represent symbol tables, symbol table
    collections, and symbol vectors.

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
// ASymbolTable ****************************************************************
{ This class represents a symbol table, which matches literal token values to
  an instance of @link(ASymbol) or one of its descendants.  Symbol tables are
  used by parsers to collect and recall symbols declared in a source file.
  Such symbols typically nclude identifiers (variable, custom type, and function
  names) as well as string literals and numeric constants.

  Instances of @link(ASymbolTable) are designed to be used at parse time, when
  it is more convenient to look up symbols by name.  Once the source has been
  parsed in its entirety and is error-free, however, it is faster to look up
  symbols by index.  For this reason, instances of @link(ASymbolTable) do not
  provide a @link(CanStream.readFrom readFrom) method to allow them to be
  rebuilt from a binary stream; instead, you should create an instance of
  @link(ASymbolVector) to do this.

  Although instances of @link(ASymbolTable) make use of the
  @link(ANode.Parent Parent) property inherited from @link(ANode), symbol tables
  are not normally collected into hierarchical lists but are instead
  kept in a vector to make symbol lookup as fast as possible.  The
  @link(ANode.Parent Parent) property allows the parent scope to be searched
  for a given symbol, in case that symbol is not found in the immediate scope.
  This allows global symbols to be found even when dealing with local scopes,
  and it also allows local symbols with the same name as a global variable
  to override their counterparts.
}
type ASymbolTable = class(AStringTree)
  public
    { Construct a new symbol table that will be a child of the specified table.

      @code(ThisParent) will become the parent table in the sense that its
      symbols will be searched if a specific symbol is not found in the new
      table.  This behavior allows variables from the parent scope to be
      "present" within the child scope.  By following the chain of references
      returned by each successive call to @link(ASymbolTable.ParentTable),
      one should eventually arrive at the symbol table which represents the
      global scope.  However, symbol tables are not actually organized into
      hierarchical trees, but rather into a vector to make symbol lookup as
      fast as possible.
    }
    constructor asChildOf(const ThisParent: ASymbolTable); reintroduce;
      virtual;

    { Determine whether or not the table has the named symbol.

      This method simply calls @link(ABinaryTree.hasLeafWithKey) and returns
      the result.  It searches only the current symbol table.
    }
    function hasSymbolNamed(const thisName: string): boolean; virtual;

    { Fetch the named symbol.

      This method first calls @link(ABinaryTree.FetchLeaf) to search the
      current symbol table.  If a match is not found and
      @code(expandSearch) is @true, then the method calls
      @link(ABinaryTree.FetchLeaf) on @link(ASymbolTable.ParentTable).

      If a match is found in the current symbol table or its parent scope,
      a reference to the symbol is returned.  Otherwise, this routine returns
      @nil.
    }
    function SymbolNamed(const thisName: string): ASymbol; virtual;

    { Add the specified symbol to the table.

      This method simply calls @link(ABinaryTree.addLeaf) and returns the
      result.
    }
    function addSymbol(const ThisSymbol: ASymbol;
      const freeOnFailure: boolean = true): boolean; virtual;

    { Insert the specified symbol into the table.

      This method simply calls @link(ABinaryTree.InsertLeaf) and then casts
      the return value as an instance of @link(ASymbol).
    }
    function InsertSymbol(const ThisSymbol: ASymbol): ASymbol; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the parent symbol table, if any.  The parent table
      represents the scope which encloses the scope represented by the current
      table.

      The reference returned by this routine should NOT be freed by the caller.
    }
    function ParentTable: ASymbolTable; virtual;
end;

// ASymbolVector ***************************************************************
{ This class represents a symbol vector, which matches symbols to an index.
  The symbol vector is designed to be read from a binary stream, where it will
  have previously been written by an instance of @link(ASymbolTable).

  Unlike @link(ASymbolTable), it is not necessary for instances of
  @link(ASymbolVector) to link to a parent scope; this is because each symbol
  read from the intermediate code stream contains an absolute reference to the
  symbol vector to which it belongs, negating the need to search through
  symbol tables in order to find it.
}
type ASymbolVector = class(AnObjectVector)
  public
    { Initializer }
    function init: boolean; override;

    { Fetch the symbol at the specified index.

      This method simply calls @link(AnObjectVector.ObjectAt) and then casts the
      result to an instance of @link(ASymbol).
    }
    function SymbolAt(const thisIndex: integer): ASymbol; virtual;
end;

// ASymbolTableVector **********************************************************
{ This class represents a symbol table vector, which organizes instances of
  @link(ASymbolTable) into a linear array.  This allows symbol tables to be
  organized into @bold(scopes), with each symbol table representing a single
  scope.  Generally speaking, there will be at least one symbol table in
  @link(ASymbolTableVector), corresponding to @link(SYMSCOPE_GLOBAL), or the
  global scope, which represents symbols that are available everywhere in a
  compiled program.  Instances of @link(ASymbolTableVector) will likely be used
  in the process of parsing source into an intermediate code file.
}
type ASymbolTableVector = class(AnObjectVector)
  public
    { Fetch the symbol table at the specified index.

      This method simply calls @link(AnObjectVector.ObjectAt) and then casts the
      result to an instance of @link(ASymbolTable).
    }
    function TableAt(const thisScope: TSymbolScope): ASymbolTable;
      virtual;

    { Push the specified symbol table onto the end of the vector.

      This method simply calls @link(AnObjectVector.pushObject).
    }
    procedure pushTable(const ThisTable: ASymbolTable); virtual;

    { Add the specified table to the vector and return its index.

      This method simply calls @link(AnObjectVector.appendObject) and
      returns the result.
    }
    function appendTable(const ThisTable: ASymbolTable): longword; virtual;
end;

// ASymbolVectorVector *********************************************************
{ This class represents a vector of symbol vectors; in other words, a collection
  of instances of @link(ASymbolVector).  It organizes symbol vectors into a
  linear array, allowing each vector to represent a single scope.  Generally
  speaking, there will be at least one symbol vector in
  @link(ASymbolVectorVector), corresponding to @link(SYMSCOPE_GLOBAL), or the
  global scope, which will contain symbols available everywhere in the
  compiled program.  Instances of @link(ASymbolVectorVector) are designed to be
  used in the process of working with the parsed code in an intermediate code
  file.
}
type ASymbolVectorVector = class(AnObjectVector)
  public
    { Initializer }
    function init: boolean; override;

    { Fetch the symbol vector at the specified index.

      This method simply calls @link(AnObjectVector.ObjectAt) and then casts the
      result to an instance of @link(ASymbolVector).
    }
    function VectorAt(const thisScope: TSymbolScope): ASymbolVector;
      virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ASymbolTable ****************************************************************
// Constructor
constructor ASymbolTable.asChildOf(const ThisParent: ASymbolTable);

begin
  inherited new;

  MyParent := ThisParent;
end;

// Determine if the table has the named symbol
function ASymbolTable.hasSymbolNamed(const thisName: string): boolean;

begin
  result := Self.hasLeafWithKey(thisName);
end;

// Fetch the named symbol
function ASymbolTable.SymbolNamed(const thisName: string): ASymbol;

begin
  // Search ourselves first
  Result := ASymbol(Self.FetchLeaf(thisName));

  // If no match was found, search our parent
  if (Result = nil) and (MyParent <> nil) then
    Result := ASymbol(ASymbolTable(MyParent).FetchLeaf(thisName));
end;

// Add the specified symbol to the table
function ASymbolTable.addSymbol(const ThisSymbol: ASymbol;
  const freeOnFailure: boolean = true): boolean;

begin
  result := Self.addLeaf(ThisSymbol, freeOnFailure);
end;

// Insert the specified symbol into the table
function ASymbolTable.InsertSymbol(const ThisSymbol: ASymbol): ASymbol;

begin
  Result := ASymbol(Self.InsertLeaf(ThisSymbol));
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the parent table
function ASymbolTable.ParentTable: ASymbolTable;

begin
  Result := ASymbolTable(MyParent);
end;

// ASymbolVector ***************************************************************
// Initializer
function ASymbolVector.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    // By default, we manage instances of ASymbolRecalled
    MyObjectType := ASymbolRecalled;
  end;
end;

// Fetch the symbol at the given index
function ASymbolVector.SymbolAt(const thisIndex: integer): ASymbol;

begin
  Result := ASymbol(Self.ObjectAt(thisIndex));
end;

// ASymbolTableVector **********************************************************
// Fetch the symbol table at the specified index
function ASymbolTableVector.TableAt(const thisScope: integer): ASymbolTable;

begin
  Result := ASymbolTable(Self.ObjectAt(thisScope));
end;

// Push the specified table onto the end of the vector
procedure ASymbolTableVector.pushTable(const ThisTable: ASymbolTable);

begin
  Self.pushObject(ThisTable);
end;

// Append the specified table and return its index
function ASymbolTableVector.appendTable(const ThisTable: ASymbolTable): longword;

begin
  result := Self.appendObject(ThisTable);
end;

// ASymbolVectorVector *********************************************************
// Initializer
function ASymbolVectorVector.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    // By default, we manage instances of ASymbolVector
    MyObjectType := ASymbolVector;
  end;
end;

// Fetch the symbol vector at the specified index
function ASymbolVectorVector.VectorAt(const thisScope: integer): ASymbolVector;

begin
  Result := ASymbolVector(Self.ObjectAt(thisScope));
end;

{$ENDIF}

