{ opcodes.inc: Matching opcodes to token strings.

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
{ This type represents an opcode, which is a way of constructing a numeric
  representation of a token string that has special meaning to a parser.  The
  internal representation of an opcode is faster for a computer to manage than
  the associated token string; as a result, almost all parsers use some kind of
  logic to match a token string to an internal representation.

  This type is defined here to make parser code more flexible.
}
type TOpcode = longword;

// AnOpcodeDictionaryEntry *****************************************************
{ This class represents an opcode dictionary entry, which matches a single
  token string to its internal representation.  The meaning and value of the
  internal representation will vary, depending on the needs of the language
  and the specification of its parser, but working with the internal
  representation is faster than working with the string representation itself.

  Instance of @link(AnOpcodeDictionaryEntry) are immutable once they have been
  constructed.
}
type AnOpcodeDictionaryEntry = class(AStringLeaf)
  protected
    { Stores the internal representation of the opcode }
    myCode: TOpcode;

  public
    { Construct a new opcode entry that binds the specified token string to
      an internal representation.
    }
    constructor binding(const tokenString: string;
      const opcode: TOpcode);

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom): it calls that routine, then checks to
      see whether @code(Other) is an instance of @link(AnOpcodeDictionaryEntry).
      If so, it copies the value of @link(AnOpcodeDictionaryEntry.code) from
      @code(Other) to @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      Likewise, it does NOT copy the left or right subtrees and so cannot be
      used to create a full copy of any descendant of @link(ABinaryLeaf).
      The copy will NOT be automatically placed in the binary tree to which
      @code(Other) belongs, if any, but the caller is free to do so, so long
      as the node's sort key does not match one that already exists in the
      tree.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the opcode, and just the opcode, to the given stream.

      This method extends the behavior inherited from
      @link(AStringLeaf.writeSelfTo).  It calls that routine, then writes
      the value of @link(AnOpcodeDictionaryEntry.code) to @code(Dest).

      See @link(AStringLeaf.writeSelfTo) for more information.
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the item from the given stream.

      This method extends the behavior inherited from
      @link(AStringLeaf.readFrom).  It calls that routine, then reads the value
      of @link(AnOpcodeDictionaryEntry.code) from @code(Source).

      See @link(AStringLeaf.readFrom) for more information.
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation ----------------------------------------------
    { Construct and return a string representation of the opcode, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(AStringLeaf.toString); it simply returns the token string that is
      bound to the opcode's internal representation.
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the internal representation of the opcode. }
    function code: TOpcode;
end;

{ This class represents a dictionary of opcodes.  It is used by
  @link(AParsedLanguage) to match token strings to their internal
  representations.
}
type AnOpcodeDictionary = class(AStringTree)
  public
    { Initializer }
    function init: boolean; override;

    { Bind the specified token string to the specified internal representation
      and insert the binding into the dictionary.

      @return(The new entry that has been inserted into the dictionary, if
      successful.  As with other descendants of @link(ABinaryTree), instances of
      @link(AnOpcodeDictionary) do not support duplicate entries.  If a binding
      for @code(tokenString) already exists in the dictionary, then this routine
      does nothing and returns @nil.)
    }
    function bind(const tokenString: string;
      const opcode: TOpcode): AnOpcodeDictionaryEntry; virtual;

    { Bind the specified token strings in the first array to the corresponding
      internal representations from the second array.

      Both arrays must be of equal length for this method to succeed.  The
      arrays should be filled such that @code(tokenStrings[1]) has its
      internal representation specified in @code(opcodes[1]), and so forth.
      This method is primarily designed to allow token strings and opcodes to
      be hard-coded as constants within the program itself.

      @return(The total number of bindings inserted into the dictionary.  This
      number may be less than @code(System.length) reports for the arrays
      passed to this method if the arrays contain duplicate entries.  This
      value will be zero (0) if the length of both arrays does not match.)
    }
    function bindSeveral(const tokenStrings: array of string;
      const opcodes: array of TOpcode): longword; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve the internal representation for the given @code(tokenString).

      This method searches through the entries in the dictionary for the
      instance of @link(AnOpcodeDictionaryEntry) that has the given
      @code(tokenString); if found, @link(AnOpcodeDictionaryEntry.code) is
      called on that instance and the value provided is returned to the caller.

      If the specified @code(tokenString) is not found, then this routine
      returns @code(TOpcode(0)).
    }
    function valueOf(const tokenString: string): TOpcode; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
/// AnOpcodeDictionaryEntry ****************************************************
/// Constructor
constructor AnOpcodeDictionaryEntry.binding(const tokenString: string;
  const opcode: TOpcode);

begin
  inherited named(tokenString);

  myCode := opcode;
end;

/// Initializer
function AnOpcodeDictionaryEntry.init: boolean;

begin
  myCode := 0;

  result := inherited init;
end;

// Make shallow copy of the other object
function AnOpcodeDictionaryEntry.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is AnOpcodeDictionaryEntry) then
    result := false;

  if result then
  begin
    myCode := AnOpcodeDictionaryEntry(Other).code;
  end;
end;

// Write the opcode, and just the opcode, to the given stream
function AnOpcodeDictionaryEntry.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine first
  result := inherited writeSelfTo(Dest);

  // Write the value of the opcode to the stream
  result := result + Dest.write(sizeof(myCode), myCode);
end;

// CanStream implementation ----------------------------------------------------
// Read the opcode from the specified stream
function AnOpcodeDictionaryEntry.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source = nil then
    exit;

  // Call the inherited routine first
  result := inherited readFrom(Source);

  // Read the value of the opcode
  result := result + Source.read(sizeof(myCode), myCode);
end;

// CanPrint implementation -----------------------------------------------------
/// Return a string representation of the opcode
function AnOpcodeDictionaryEntry.toString: string;

begin
  if myName <> '' then
    result := myName;
end;

// Properties ------------------------------------------------------------------
/// Retrieve the value of the opcode
function AnOpcodeDictionaryEntry.code: TOpcode;

begin
  result := myCode;
end;

// AnOpcodeDictionary **********************************************************
// Initializer
function AnOpcodeDictionary.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := AnOpcodeDictionaryEntry;
  end;
end;

/// Inserts the specified entry into the dictionary
function AnOpcodeDictionary.bind(const tokenString: string;
  const opcode: TOpcode): AnOpcodeDictionaryEntry;

begin
  Result := AnOpcodeDictionaryEntry.binding(tokenString, opcode);

  if (Result <> nil) and (Result <> inherited insert(Result)) then
  begin
    Result.free;
    Result := nil;
  end;
end;

/// Inserts a series of entries into the dictionary
function AnOpcodeDictionary.bindSeveral(const tokenStrings: array of string;
  const opcodes: array of TOpcode): longword;

var
  // A loop counter
  i: longword;

begin
  result := 0;

  if System.length(tokenStrings) = System.length(opcodes) then
  begin
    for i := 0 to System.high(tokenStrings) do
      if Self.bind(tokenStrings[i], opcodes[i]) <> nil then
        inc(result);
  end;
end;

// Properties --------------------------------------------------------------
/// Matches the specified token string with its opcode
function AnOpcodeDictionary.valueOf(const tokenString: string): TOpcode;

var
  // Refers to the entry in question
  Entry: AnOpcodeDictionaryEntry;

begin
  result := 0;

  Entry := AnOpcodeDictionaryEntry(Self.fetch(tokenString));

  if Entry <> nil then
    result := Entry.code;
end;
{$ENDIF}


