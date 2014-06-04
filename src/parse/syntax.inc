{ syntax.inc: A way to programatically define syntax rules for a parser to
    follow.

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
{ This type represents a dynamic array of opcodes.  It is primarly used by
  @link(ASyntaxRule).
}
type TOpcodeList = array of TOpcode;

// ASyntaxRule *****************************************************************
{ This class represents a single syntax rule, which in its simplest form is
  a set of opcodes.  The set can be used by a parser to determine which
  tokens are allowed in a given situation, which tokens are not allowed in
  a given situation, or the order in which tokens must be provided in a given
  situation.

  Note that syntax rules can only grow; they cannot otherwise be modified,
  because it is assumed that they will be defined at compile-time as part of
  a language specification.

  Instances of @link(ASyntaxRule) descend from @link(AStringLeaf) so that they
  can be gathered together into lists of rules, such as @link(ASyntaxRuleset),
  that define the grammar for a particular parsed language.
}
type ASyntaxRule = class(AStringLeaf)
  protected
    { Stores the opcodes associated with the rule }
    myComponents: TOpcodeList;

  public
    { Construct a new rule with the specified sort key and which incorporates
      the specified opcodes.

      The opcodes provided by @code(thisList) are used to specify the opcodes
      that apply to the rule.
    }
    constructor withSortKey(const thisKey: TSortKey;
      const thisList: array of TOpcode); reintroduce; virtual;

    { Construct a new rule with the specified sort key and which incorporates
      the specified opcodes.

      The hash value of @code(thisKey), as determined by a call to
      @link(Charstring.hashValueOf), is used as the sort key of the rule.

      The opcodes provided by @code(thisList) are used to specify the opcodes
      that apply to the rule.
    }
    constructor named(const thisKey: string;
      const thisList: array of TOpcode); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Free the rule and the list of opcodes associated with it.

      This method is called automatically when @link(TObject.free) is called on
      an instance of @link(ASyntaxRule).
    }
    destructor destroy; override;

    { Add the specified opcode to the rule.

      The new opcode is placed at the end of the rule.  Unlike instances of
      @link(ABinaryTree), instances of @link(ASyntaxRule) allow duplicate
      entries.

      @return(The index of the new entry.)
    }
    function add(const opcode: TOpcode): longint; virtual;

    { Add the specified opcodes to the rule.

      The new opcodes are placed at the end of the rule.  Unlike instances of
      @link(ABinaryTree), instances of @link(ASyntaxRule) allow duplicate
      entries.

      @return(The total number of items added to the rule.  This value will
      be zero (0) if @code(System.length) reports that @code(opcodeList) has a
      length of zero.)
    }
    function addSeveral(const opcodeList: array of TOpcode): longint; virtual;

    { Push the specified opcode onto the end of the rule's list of opcodes.

      This method is similar in function to @link(ASyntaxRule.add), except that
      it does not return the index of the new entry.
    }
    procedure push(const opcode: TOpcode); virtual;

    { Push the specified opcodes onto the end of the rule's list of opcodes.

      This method is similar in function @link(ASyntaxRule.addSeveral), except
      that it does not return the total number of items added to the rule.
    }
    procedure pushSeveral(const opcodeList: array of TOpcode); virtual;

    { Determine whether the specified opcode is a component of the rule.

      This method performs a sequential search of the opcodes specified for
      the rule and returns @true if it encounters a value that matches
      the specified @code(opcode).  Otherwise, it returns @false.
    }
    function has(const opcode: TOpcode): boolean; virtual;

    { Determine whether one of the specified opcodes is a component of the
      rule.

      This method performs a sequential search of the opcodes specified for
      the rule and returns the first opcode in @code(opcodeList) which occurs
      as part of the rule.  If no match is found for any of the opcodes in
      @code(opcodeList), this method returns @code(TOpcode(0)).
    }
    function hasAnyOf(const opcodeList: array of TOpcode): TOpcode; virtual;

    { Find the first occurrence of the specified opcode in the rule, optionally
      beginning the search after the specified index.

      This method performs a sequential search of the components of the rule
      and returns the index of the first occurrence of @code(opcode) that it
      finds.  The search begins at index zero (0) unless @code(afterIndex) is
      specified.  If @code(afterIndex) specifies a value that is greater than
      the value of @link(ASyntaxRule.census), then this routine will do nothing
      and return -1.

     @return(The index of the rule component that matches @code(opcode), if
     found; -1 otherwise.)
    }
    function find(const opcode: TOpcode; afterIndex: longint = 0): longint;
      virtual;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom): it calls that routine, then checks to
      see whether @code(Other) is an instance of @link(ASyntaxRule).  If so,
      it copies the values in @link(ASyntaxRule.components) from @code(Other) to
      @code(Self).

      The behavior of this routine allows a full copy of @code(Other) to be
      added to @code(Self), since no components of @code(Self) are deleted by
      this routine.

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      Likewise, it does NOT copy the left or right subtrees and so cannot be
      used to create a full copy of any descendant of @link(ABinaryLeaf).
      The copy will NOT be automatically placed in the binary tree to which
      @link(Other) belongs, if any, but the caller is free to do so, so long
      as the node's sort key does not match one that already exists in the
      tree.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the rule, and just the rule, to the specified stream.

      This method writes the value of @code(Self.census) to @code(Dest),
      followed by the opcodes belonging to the rule, then calls the inherited
      routine.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the rule from the specified stream.

      This method reads the number of opcodes to retrieve from the stream,
      the reads the opcodes themselves before calling the inherited routine.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // Properties --------------------------------------------------------------
    { Calculate the total number of components in the rule.

      This method simply returns the value of @code(System.length) when applied
      to @link(ASyntaxRule.components).
    }
    function census: longint; reintroduce; virtual;

    { Return a reference to the list of opcodes that make up the rule. }
    function components: TOpcodeList; virtual;
end;

{ This class represents a collection of syntax rules which, taken together,
  help to control how a parser processes source.

  The collection of rules is implemented as a binary tree: rules can be added
  and removed, and can be retrieved by their sort key, which can be a hashed
  string or an integer value.  Rulesets can also be written to and read from
  arbitrary streams.  For more information, see @link(AStringTree).
}
type ASyntaxRuleset = class(AStringTree)
  public
    // Initializer
    function init: boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the rule with the specified sort key.

      This method calls @link(ABinaryTree.fetch) on itself, then returns a
      reference to the resulting rule, if found.  If the rule is not found, then
      this method returns @nil.

      The caller should NOT attempt to free the reference returned by this
      routine.
    }
    function Rule(const thisKey: TSortKey): ASyntaxRule; virtual;

    { Retrieve a reference to the rule with the specified name.

      The value of @code(thisKey) is hashed by calling
      @link(Charstring.hashValueOf); this value is then passed to
      @link(ABinaryTree.fetch).  A reference to the matching rule, if found, is
      returned to the caller.  If the rule is not found, then this method
      returns @nil.

      The caller should NOT attempt to free the reference returned by this
      routine.
    }
    function Rule(const thisKey: string): ASyntaxRule; virtual;
end;

// Default syntax rules recognized by a parser
const
  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule so
    that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: rules defining basic token types and indicating which tokens are
    allowed to begin and end a statement.

    This constant is used to define an invalid rule (no rule).
  }
  RULE_NONE = 0;

  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule
    so that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: a rules defining basic token types and a rule to indicate which
    tokens are allowed to begin and end a statement.

    This constant is used to define the basic token categories that a scanner
    recognizes.  It is inserted automatically by the base code in
    @link(AParsedLanguage) every time an instance of that class, or one of its
    descendants, is instantiated.
  }
  RULE4_BASIC_TOKENS = RULE_NONE + 1;

  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule
    so that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: a rules defining basic token types and a rule to indicate which
    tokens are allowed to begin and end a statement.

    This constant is used to define the tokens that may begin a statement.  It
    is not automatically used by the base instance of @link(AParsedLanguage),
    but is provided here as a way to standardize the way in which one of the
    more fundamental rules of a parsed language is defined.
  }
  RULE4_BEGINNING_STATEMENTS = RULE4_BASIC_TOKENS + 1;

  { Rules in @link(ASyntaxRuleset) can be retrieved by an arbitrary name or
    value; however, it is common to define a symbolic constant for each rule
    so that the rule name or value does not have to be hard-coded.  In addition,
    there are some basic rules that will probably be common across all types of
    parsers: a rules defining basic token types and a rule to indicate which
    tokens are allowed to begin and end a statement.

    This constant is used to define the tokens that may end a statement.  It
    is not automatically used by the base instance of @link(AParsedLanguage),
    but is provided here as a way to standardize the way in which one of the
    more fundamental rules of a parsed language is defined.
  }
  RULE4_ENDING_STATEMENTS = RULE4_BEGINNING_STATEMENTS + 1;
{$ENDIF}
{$IFDEF ReadImplementation}
// ASyntaxRule *****************************************************************
// Constructor
constructor ASyntaxRule.withSortKey(const thisKey: TSortKey;
  const thisList: array of TOpcode);

begin
  inherited withSortKey(thisKey);

  Self.addSeveral(thisList);
end;

// Constructor
constructor ASyntaxRule.named(const thisKey: string;
  const thisList: array of TOpcode);

begin
  inherited named(thisKey);

  Self.addSeveral(thisList);
end;

// Initializer
function ASyntaxRule.init: boolean;

begin
  System.setLength(myComponents, 0);

  result := inherited init;
end;

// Destructor
destructor ASyntaxRule.destroy;

begin
  // Free the list of rule components
  System.setLength(myComponents, 0);

  inherited destroy;
end;

// Add the specified opcode to the rule
function ASyntaxRule.add(const opcode: TOpcode): longint;

begin
  // Grow the list
  System.setLength(myComponents, Self.census + 1);
  // Store the new item
  myComponents[Self.census] := opcode;
  // Return the new index
  result := Self.census - 1;
end;

// Add several opcodes to the rule
function ASyntaxRule.addSeveral(const opcodeList: array of TOpcode): longint;

var
  // Stores the length of the components list before new opcodes are added
  lengthBefore: longint;

begin
  result := System.length(opcodeList);

  // If no opcodes were specified, then nothing needs to be done
  if result = 0 then
    exit;

  // Grow the list
  lengthBefore := Self.census;
  System.setLength(myComponents, lengthBefore + result);

  // Copy
  MemorySpan.copy(@opcodeList[0], @myComponents[lengthBefore],
    sizeof(TOpcode) * result);
end;

// Push an opcode onto the list
procedure ASyntaxRule.push(const opcode: TOpcode);

begin
  Self.add(opcode);
end;

// Push several opcodes onto the list
procedure ASyntaxRule.pushSeveral(const opcodeList: array of TOpcode);

begin
  Self.addSeveral(opcodeList);
end;

// Determine whether the opcode exists in the list
function ASyntaxRule.has(const opcode: TOpcode): boolean;

begin
  result := Self.find(opcode) <> -1;
end;

// Determine whether any of the opcodes exists in the list
function ASyntaxRule.hasAnyOf(const opcodeList: array of TOpcode): TOpcode;

var
  // Loop counters
  i, j: longint;

begin
  result := 0;

  if System.length(opcodeList) = 0 then
    exit;


  // Search through the list of components
  for i := 0 to (Self.census - 1) do
  begin
    // Search through the list of opcodes
    for j := 0 to (System.length(opcodeList) - 1) do
      // If a match is found, return it and exit
      if myComponents[i] = opcodeList[j] then
      begin
        result := myComponents[i];
        exit;
      end;
  end;
end;

// Find the first occurrence of the specified opcode
function ASyntaxRule.find(const opcode: TOpcode;
  afterIndex: longint = 0): longint;

var
  // Loop counter
  i: longint;

begin
  result := -1;

  // We cannot search beyond the end of the list
  if afterIndex >= Self.census then
    exit;

  // Loop through the list
  for i := afterIndex to (Self.census - 1) do
    if myComponents[i] = opcode then
    begin
      result := i;
      break;
    end;
end;

// Construct a shallow copy of the other object
function ASyntaxRule.shallowCopyFrom(const Other: AnObject): boolean;

var
  // Stores the offset of the first new component in the list
  firstNewComponent: longword;
  // Stores the size of the new area allocated
  newBufferSize: longword;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ASyntaxRule) then
    result := false;

  if result then
  begin
    firstNewComponent := Self.census;
    newBufferSize := ASyntaxrule(Other).census * sizeof(TOpcode);

    // Increase the size of our component list to hold the new components
    System.setLength(myComponents, Self.census + ASyntaxRule(Other).census);
    // Now copy the items
    MemorySpan.copy(@ASyntaxRule(Other).myComponents[0],
      @myComponents[firstNewComponent], newBufferSize);
  end;
end;

// Write the rule, and just the rule, to the specified stream
function ASyntaxRule.writeSelfTo(const Dest: AStream): TStreamIOSize;

var
  // Stores the current length of the rule
  myLength: longint;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Write the total number of components in the rule
    myLength := Self.census;
    result := Dest.write(sizeof(myLength), myLength);

    // Write the components themselves
    result := result + Dest.write(myLength * sizeof(TOpcode), myComponents[0]);
  end;

  result := result + inherited writeSelfTo(Dest);
end;

// CanStream implementation ----------------------------------------------------
// Read the rule from the specified stream
function ASyntaxRule.readFrom(const Source: AStream): TStreamIOSize;

var
  // Stores the number of components to read from the stream
  opcodeCount: longint;
  // Stores the components read from the stream
  newComponents: TOpcodeList;

begin
  result := 0;

  if Source <> nil then
  begin
    // Read the total number of components from the stream
    opcodeCount := 0;
    result := Source.read(sizeof(opcodeCount), opcodeCount);

    System.setLength(newComponents, opcodeCount);

    if opcodeCount > 0 then
    begin
      // Read the new components from the stream
      result := result + Source.read(opcodeCount * sizeof(TOpcode),
        newComponents[0]);
      // Add the components
      Self.addSeveral(newComponents);
      System.setLength(newComponents, 0);
    end;
  end;

  result := result + inherited readFrom(Source);
end;

// Properties ------------------------------------------------------------------
// Return the number of components in the rule
function ASyntaxRule.census: longint;

begin
  result := System.length(myComponents);
end;

// Return a reference to the list of components
function ASyntaxRule.components: TOpcodeList;

begin
  result := myComponents;
end;

// ASyntaxRuleset **************************************************************
// Initializer
function ASyntaxRuleset.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := ASyntaxRule;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the specified rule
function ASyntaxRuleset.Rule(const thisKey: TSortKey): ASyntaxRule;

begin
  Result := ASyntaxRule(Self.fetch(thisKey));
end;

// Retrieve the named rule
function ASyntaxRuleset.Rule(const thisKey: string): ASyntaxRule;

begin
  Result := ASyntaxRule(Self.fetch(thisKey));
end;
{$ENDIF}

