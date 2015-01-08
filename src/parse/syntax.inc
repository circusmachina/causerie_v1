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

  Instances of @link(ASyntaxRule) descend from @link(ABinaryLeaf) so that they
  can be gathered together into lists of rules, such as @link(ASyntaxRuleset),
  that define the grammar for a particular parsed language.
}
type ASyntaxRule = class(ABinaryLeaf)
  protected
    { Stores the opcodes associated with the rule }
    myComponents: TOpcodeList;

  public
    { Construct a new rule with the specified sort key and which incorporates
      the specified opcodes.

      The opcodes provided by @code(thisList) are used to specify the opcodes
      that apply to the rule.
    }
    constructor withIdentifier(const thisKey: TSortKey;
      const thisList: array of TOpcode); virtual;

    { Initializer }
    function init: boolean; override;

    { Free the rule and the list of opcodes associated with it.

      This method is called automatically when @code(TObject.free) is called on
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
      @code(Other) belongs, if any, but the caller is free to do so, so long
      as the node's sort key does not match one that already exists in the
      tree.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Calculate the number of bytes required to stream the rule, and just the
      rule.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.selfStreamingLength): it calls the inherited routine,
      then adds the number of bytes required to store
      @link(ASyntaxRule.census Self.census) and the opcodes themselves.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the rule, and just the rule, to the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.writeSelfTo): it first calls the inherited routine, then
      writes the value of @link(ASyntaxRule.census Self.census) to @code(Dest)
      followed by the opcodes belonging to the rule.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the rule from the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom): it first calls the inherited routine, then
      reads the number of opcodes to retrieve from the stream and the opcodes
      themselves.

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
  and removed, and can be retrieved by their sort key, which can be an integer
  value.  Rulesets can also be written to and read from arbitrary streams.  For
  more information, see @link(ABinaryTree).
}
type ASyntaxRuleset = class(ABinaryTree)
  public
    // Initializer
    function init: boolean; override;

    { Add the given rule to the ruleset.

      This method simply calls @link(ABinaryTree.addLeaf) on itself and returns
      the result.  It is defined for convenience.
    }
    function addRule(const ThisRule: ASyntaxRule;
      const freeOnFailure: boolean = false): boolean; virtual;

    { Add a rule with the specified sort key to the ruleset.

      This method constructs an instance of @link(ASyntaxRule) that is
      identified by @code(thisKey) by calling @link(ASyntaxRule.withSortKey).
      It then attempts to add the rule to the ruleset by calling
      @link(ASyntaxRuleset.addRule) on @code(Self).

      @return(@true if the new rule was successfully created and inserted;
      @false if not).
    }
    function addNewRule(const thisKey: TSortKey;
      const thisList: array of TOpcode): boolean; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the rule with the specified sort key.

      This method calls @link(ABinaryTree.fetchLeaf) on itself, then returns a
      reference to the resulting rule, if found.  If the rule is not found, then
      this method returns @nil.

      The caller should NOT attempt to free the reference returned by this
      routine.
    }
    function Rule(const thisKey: TSortKey): ASyntaxRule; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ASyntaxRule *****************************************************************
// Constructor
constructor ASyntaxRule.withIdentifier(const thisKey: TSortKey;
  const thisList: array of TOpcode);

begin
  inherited withSortKey(thisKey);

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
  // Loop counter
  i: longint;

begin
  result := 0;

  if System.length(opcodeList) = 0 then
    exit;

  // Search through the list of components
  for i := 0 to (System.length(opcodeList) - 1) do
    if Self.find(myComponents[i]) <> -1 then
    begin
      result := myComponents[i];
      break;
    end;
end;

// Find the first occurrence of the specified opcode
function ASyntaxRule.find(const opcode: TOpcode;
  afterIndex: longint = 0): longint;

var
  // Loop counter
  i: longint;
  // Stores a reference to a sibling rule
  Sibling: ASyntaxRule;

begin
  result := -1;

{$IFDEF DEBUG}
  writeln(SysUtils.format('Rule %.8X: Searching for opcode %.8X after index %d...', [
    mySortKey, opcode, afterIndex
  ]));
{$ENDIF}

  // We cannot search beyond the end of the list
  if afterIndex >= Self.census then
    exit;

  // Loop through the list
  for i := afterIndex to (Self.census - 1) do
    if myComponents[i] = opcode then
    begin
      result := i;
      break;
    end

    // Check for a reference to another rule
    else if ((myComponents[i] and TOKCAT_CATMASK) = TOKCAT_RULE) and
      (MyParent <> nil) then
    begin
      // Fetch the rule indicated
      Sibling := nil;
      if MyParent is ASyntaxRuleset then
        Sibling := ASyntaxRuleset(MyParent).Rule(myComponents[i]);

      // See if that rule has the desired opcode
      // Sibling <> Self: do what we can to prevent circular references
      if (Sibling <> nil) and (Sibling <> Self) and (Sibling.has(opcode)) then
      begin
        result := i;
        break;
      end;
    end;

{$IFDEF DEBUG}
  if result <> - 1 then
    writeln(SysUtils.format('Rule %.8X: Opcode found!', [
      mySortKey
    ]))

  else
    writeln(SysUtils.format('Rule %.8X: Opcode not found', [
      mySortKey
    ]));
{$ENDIF}
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

// Calculate the number of bytes required to stream the rule
function ASyntaxRule.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine
  result := inherited selfStreamingLength;

  // Include ourselves
  inc(result,
    sizeof(longint) + (System.length(myComponents) * sizeof(TOpcode)));
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
    result := inherited writeSelfTo(Dest);

    // Write the total number of components in the rule
    myLength := Self.census;
    result := result + Dest.write(sizeof(myLength), myLength);

    // Write the components themselves
    result := result + Dest.write(myLength * sizeof(TOpcode), myComponents[0]);
  end;
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
    result := inherited readFrom(Source);

    // Read the total number of components from the stream
    opcodeCount := 0;
    result := result + Source.read(sizeof(opcodeCount), opcodeCount);

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

// Add a syntax rule to the ruleset
function ASyntaxRuleset.addRule(const ThisRule: ASyntaxRule;
  const freeOnFailure: boolean = false): boolean;

begin
  result := Self.addLeaf(ThisRule, freeOnFailure);

  if result then
    // Set the parent of the rule, for use with ASyntaxRule.find()
    ThisRule.MyParent := Self;
end;

// Add a new syntax rule to the ruleset
function ASyntaxRuleset.addNewRule(const thisKey: TSortKey;
  const thisList: array of TOpcode): boolean;

var
  // Refers to the new rule
  NewRule: ASyntaxRule;

begin
  result := false;

  NewRule := ASyntaxRule.withIdentifier(thisKey, thisList);
  if NewRule = nil then
    exit;

  // Attempt to add the rule, and free it on failure
  result := Self.addRule(NewRule, true);
end;

// Properties ------------------------------------------------------------------
// Retrieve the specified rule
function ASyntaxRuleset.Rule(const thisKey: TSortKey): ASyntaxRule;

begin
  Result := ASyntaxRule(Self.fetchLeaf(thisKey));
end;
{$ENDIF}

