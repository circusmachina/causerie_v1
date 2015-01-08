{ stringtrees.inc: Binary trees to manage string keys.

  This file is part of classwork.pp, a library of basic and useful classes.

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
// AStringLeaf *****************************************************************
{ This class represents a node in a binary tree that keeps the string used
  to define its sort key.  As with @link(ABinaryLeaf), instances of
  @link(AStringLeaf) use the hash value of a given string, as calculated by
  @link(Charstring.hashValueOf), to determine their sort key.
}
type AStringLeaf = class(ABinaryLeaf)
  protected
    { Stores the string used to generate the sort key for the node }
    myName: string;

  public
    { Construct a new node that uses the hash value of the specified string as
      its sort key.

      The implementation of this method is identical to that provided by
      @link(ABinaryLeaf.withSortKey), except that @code(thisKey) is stored
      internally by the node.  The string can be retrieved by calling
      @link(AStringLeaf.name).
    }
    constructor named(const thisKey: string); override;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(ABinaryLeaf.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AStringLeaf).  If so,
      it copies the value of @link(AStringLeaf.name) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).

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

    { Calculate the number of bytes required to stream the node.

      This method builds upon the behavior inherited from
      @link(ABinaryLeaf.selfStreamingLength); it calls the inherited routine,
      then calculates the number of bytes required to write its name and
      adds that to the result, which is then returned to the user.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the node, and just the node, to the specified stream.

      This method builds upon the behavior inherited from
      @link(ABinaryLeaf.writeSelfTo): it first calls the inherited routine, then
      writes the value of @link(AStringLeaf.name) to the stream by calling
      @link(AStream.writeString) on @code(Dest).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the node from the specified stream.

      This method builds upon the behavior inherited from
      @link(ABinaryLeaf.readFrom): it calls the inherited routine first, then
      reads the value of @link(AStringLeaf.name) from the stream by calling
      @link(AStream.readString) on @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the node, suitable for
      printing to a text-based console or device.

      In accordance with the criteria described by
      @link(APrintingObject.toString), this method returns a string which
      contains the index of the node and the string used to define its sort
      key.  The format of the string returned is controlled by
      @link(stlfStringRepresentation).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the name of the node.  This is the string that was used at
      construction time to generate the sort key of the node.
    }
    function name: string; override;
end;

// AStringTree *****************************************************************
{ This class represents a binary tree that is comprised of instances of
  @link(AStringLeaf).  The functionality provided by this class is identical to
  that of @link(ABinaryTree), except that
  @link(ABinaryTree.readFrom AStringTree.readFrom) reads instances of
  @link(AStringLeaf) instead of instances of @link(ABinaryLeaf).
}
type AStringTree = class(ABinaryTree)
  public
    // Initializer
    function init: boolean; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the way a representation of @link(AStringLeaf) is
    constructed by @link(AStringLeaf.toString).

    The integer placeholder will be filled by the index of the node, and the
    string placeholder will be filled with the name of the node, as returned by
    a call to @link(AStringLeaf.name).
  }
  stlfStringRepresentation = '[%d]: "%s"';
{$ENDIF}
{$IFDEF ReadImplementation}
// AStringLeaf *****************************************************************
{* Constructs a new instance of AStringLeaf.  The hash value of \p thisName is
   used to generate a sort key for the new instance.

   \return A new instance of AStringLeaf on success; #nil on failure.
*}
constructor AStringLeaf.named(const thisKey: string);

begin
  inherited named(thisKey);

  myName := thisKey;
end;

{* Initializes a new instance of AStringLeaf.  This routine sets
   AStringLeaf::name() to an empty string and then calls the inherited routine.
   In the default implementation, therefore, this method always returns #true.

   \return #true if initialization was successful; #false if not.
*}
function AStringLeaf.init: boolean;

begin
  myName := '';
  result := inherited init;
end;

// Construct a shallow copy of the given object
function AStringLeaf.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AStringLeaf) then
    result := false;

  if result then
  begin
    myName := AStringLeaf(Other).name;
  end;
end;

// Calculate the number of bytes required to stream the node
function AStringLeaf.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine first
  result := inherited selfStreamingLength;

  // Add the length of our string + length specifier + length checksum
  inc(result, System.length(myName) + (sizeof(TStreamIOSize) * 2));
end;

// Write the node, and just the node, to the given stream
function AStringLeaf.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    result := inherited writeSelfTo(Dest);
    result := result + Dest.writeString(myName);
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the node from the given stream
function AStringLeaf.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    result := inherited readFrom(Source);
    result := result + Source.readString(myName);
  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the leaf.  This string contains the
   name of the string and its index; these values are inserted into the string
   as specified by #stlfStringRepresentation.

   \return A string representation of the leaf, as described above.
*}
function AStringLeaf.toString: string;

begin
  result := SysUtils.format(stlfStringRepresentation, [
    myIndex, myName
  ]);
end;

// Properties ------------------------------------------------------------------
{* Retrieves the name of the leaf.  This will be either the name assigned to the
   leaf when it was constructed (by AStringLeaf::named()) or, if no such name
   was assigned, the name of the class.

   \return The name of the leaf, as described above.
*}
function AStringLeaf.name: string;

begin
  result := myName;
end;

// AStringTree *****************************************************************
// Initializer
function AStringTree.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := AStringLeaf;
  end;
end;

{$ENDIF}
