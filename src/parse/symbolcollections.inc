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

  Symbol tables can write themselves to a stream and can be recalled into
  instances of @link(ASymbolVector).
}
type ASymbolTable = class(AStringTree)
  public
    { Initializer }
    function init: boolean; override;

    { Fetch the named symbol.

      This method simply calls @link(ABinaryTree.fetchLeaf) and then casts the
      result to an instance of @link(ASymbol).
    }
    function SymbolNamed(const thisName: string): ASymbol; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
{$ENDIF}

