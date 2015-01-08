{ dictionaries.inc: A pseudo-dictionary class to match strings with variant
    values.

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
// ADictonaryEntry *************************************************************
{ This class represents a dictionary entry, which is an item that is indexed
  by a string key.  The value of the item is represented by a variant object,
  so as enable the dictionary element to store the widest possible array of
  values.

  Unlike a typical dictionary, the item here is not associated with a
  hash lookup table; instead, it is treated as a node in a binary tree.
}
type ADictionaryEntry = class(AMappingEntry)
  protected
    { Stores the value of the entry }
    myValue: Variant;

  public
    { Construct a new entry that associates the specified name with the
      specified variant value.  The value of @code(thisName) will be hashed
      to determine the sort key of the dictionary entry when it is inserted
      into a dictionary.
    }
    constructor named(const thisName: string; const thisValue: Variant);
      reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom): it calls that routine, then checks to
      see whether @code(Other) is an instance of @link(ADictionaryEntry).  If
      so, it copies the value of @link(ADictionaryEntry.value) from @code(Other)
      to @code(Self), overwriting the value in @code(Self).

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

    { Calculate the number of bytes required to stream the node, and just the
      node.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.selfStreamingLength): it calls the inherited method,
      then adds the number of bytes required to store an instance of
      @code(TVarData).
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the node, and just the node, to the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.writeSelfTo): it first calls the inherited routine,
      then writes the value of the entry by converting it to an instance of
      @code(TVarData).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

  // CanStream implementation --------------------------------------------------
    { Read the node from the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom): it calls the inherited routine first, then
      reads the value of the entry by treating it as an instance of
      @code(TVarData) when reading it from @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the value associated with the entry. }
    function value: Variant; virtual;

    { Sets the value of the entry.

      @return(The previous value of the entry.)
    }
    function setValue(const newValue: Variant): Variant; virtual;

    { Retrieve the value of the entry as a string.

      This method calls @code(Variants.varToStr) on
      @link(ADictionaryEntry.value Self.value) and returns the result.
    }
    function valueAsString: string; override;

    { Set the value of the entry from the given string.

      This method calls @link(ADictionaryEntry.setValue Self.setValue) with
      @code(newValue) and returns the result formatted as a string.
    }
    function setValueFromString(const newValue: string): string; override;
end;

// ADictionary *****************************************************************
{ This class represents a dictionary, which is a list of items that are indexed
  by a string key.  The overall functionality of the dictionary is similar to
  that provided by Python and other languages that implement such objects;
  however, the dictionary is implemented as a binary tree rather than a table
  of hashed objects.

  As with instances of @link(ABinaryTree), the dictionary is capable of
  looking up entries and streaming those entries to and from arbitrary streams.
  The dictionary also supports condensing itself into a delimited string and
  iterating over its keys and values.
}
type ADictionary = class(AMapping)
  public
    { Initializer }
    function init: boolean; override;

    { Construct a list of all entry values in the dictionary.

      This method proceeds sequentially through all entries in the dictionary,
      obtaining the value of each.  The values are inserted into an instance of
      @link(AVariantList) which is constructed for this purpose.

      @return(An instance of @link(AVariantList) that contains the values of
      all dictionary entries.  This list should be freed by the caller when it
      is no longer required.  If there are no entries in the dictionary, an
      empty variant list is returned.)
    }
    function Values: AVariantList; virtual;

    { Add the specified key and value to the dictionary.

      As with other instances of @link(ABinaryTree), @link(ADictionary) does
      not allow duplicate entries; consequently, if @code(thisKey) already
      exists in the dictionary, its value will be updated with @code(thisValue).
      Otherwise, a new entry is created.

      @return(A reference to the dictionary entry that has been created and
      inserted or, if the entry already existed, the existing entry.  The caller
      should NOT attempt to free this reference; that will be done when the
      dictionary is destroyed.)
    }
    function add(const thisKey: string;
      const thisValue: Variant): ADictionaryEntry; reintroduce; virtual;

  // Properties ----------------------------------------------------------------
    { Retrieve the value of the named item.

      This method retrieves the instance of @link(ADictionaryEntry) that has
      the specified @code(key); it then returns the value obtained by calling
      @link(ADictionaryEntry.value) on that entry.

      If no entry that matches @code(key) is found in the dictionary, this
      routine returns @code(Variants.null).
    }
    function valueOf(const key: string): Variant; virtual;

    { Retrieve the value of the named item.

      This method retrieves the instance of @link(ADictionaryEntry) that has
      the specified @code(key); it then returns the value obtained by calling
      @link(ADictionaryEntry.value) on that entry.

      If no entry that matches @code(key) is found in the dictionary, this
      routine returns @code(defaultValue).
    }
    function valueOf(const key: string;
      const defaultValue: Variant): Variant; virtual;

    { Set the value of the named item.

      This method retrieves the instance of @link(ADictionaryEntry) that has the
      specified @code(key); it then sets the value by calling
      @link(ADictionaryEntry.setValue) on that entry.

      If no entry that matches @code(key) is found in the dictionary, then this
      method will add a new item.

      @return(The previous value of the specified item, if it was found;
      @code(Variants.null) otherwise.)
    }
    function setValueOf(const key: string;
      const thisValue: Variant): Variant; virtual;

    { Retrieves the value at the specified index in the dictionary.

      Internally, this method calls @link(ABinaryTree.LeafAtIndex) on itself; if
      that method returns a node, then @link(ADictionaryEntry.value) is called
      on it.

      @return(The value of the node at the specified @code(index), if found;
      @code(Variants.null) otherwise.)
    }
    function valueAt(const thisIndex: TNodeAbsoluteIndex): Variant; virtual;

    { Set the value of the entry at the specified index in the dictionary.

      Internally, this method calls @link(ABinaryTree.LeafAtIndex) on itself; if
      that method returns a node, then @link(ADictionaryEntry.setValue) is
      called on it.

      @return(The previous value of the node at the specified @code(index), if
      found; @code(Variants.null) otherwise.)
    }
    function setValueAt(const thisIndex: TNodeAbsoluteIndex;
      const newValue: Variant): Variant; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ADictionaryEntry ************************************************************
{* Constructs and initializes a new dictionary entry using the specified name
   and value.

   \return A new instance of the dictonary entry on success; #nil on failure.
*}
constructor ADictionaryEntry.named(const thisName: string;
  const thisValue: Variant);

begin
  inherited named(thisName);
  myValue := thisValue;
end;

{* Initializes a new dictionary entry.  This routine sets the value of
   ADictonaryEntry.value to Null and then calls the inherited routine.

   \return #true if initialization was successful; #false if not.
*}
function ADictionaryEntry.init: boolean;

begin
  myValue := Variants.null;
  result := inherited init;
end;

// Construct a shallow copy of the other object.
function ADictionaryEntry.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ADictionaryEntry) then
    result := false;

  if result then
  begin
    myValue := ADictionaryEntry(Other).value;
  end;
end;

// Calculate the number of bytes required to stream the node
function ADictionaryEntry.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine
  result := inherited selfStreamingLength;

  // Include ourselves
  inc(result, sizeof(TVarData));
end;

// Write the node, and just the node, to the specified stream
function ADictionaryEntry.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    result := inherited writeSelfTo(Dest);
    result := result + Dest.write(sizeof(TVarData), TVarData(myValue));
  end;
end;

// CanStream implementation ----------------------------------------------------
{* Reads the node from the specified stream.

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function ADictionaryEntry.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    result := inherited readFrom(Source);
    result := result + Source.read(sizeof(TVarData), TVarData(myValue));
  end;
end;

// Properties ------------------------------------------------------------------
/// Retrieves the value of the entry
function ADictionaryEntry.value: Variant;

begin
  result := myValue;
end;

/// Sets the value of the entry
function ADictionaryEntry.setValue(const NewValue: Variant): Variant;

begin
  result := myValue;
  myValue := NewValue;
end;

// Retrieve the value of the entry as a string
function ADictionaryEntry.valueAsString: string;

begin
  result := Variants.varToStr(myValue);
end;

// Set the value of the entry from a string
function ADictionaryEntry.setValueFromString(const newValue: string): string;

begin
  result := Variants.varToStr(Self.setValue(newValue));
end;

// ADictionary *****************************************************************
// Initializer
function ADictionary.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := ADictionaryEntry;
  end;
end;


{* Retrieves a list of dictionary values.  This routine calls
   ADictionaryEntry.implodeValueInto() on its root node in order to obtain a
   list of every dictionary value.

   \return A list of dictionary values.  If the dictionary contains no entries,
   this routine will return #nil.
*}
function ADictionary.Values: AVariantList;

var
  // Refers to an entry from the dictionary
  ThisEntry: ADictionaryEntry;

begin
  Result := AVariantList.new;

  ThisEntry := ADictionaryEntry(MyRoot);

  while ThisEntry <> nil do
  begin
    Result.push(ThisEntry.value);

    ThisEntry := ADictionaryEntry(ThisEntry.Next);
  end;
end;

{* Creates a new dictionary entry with the specified key and value.

   \return The new dictionary entry on success; #nil on failure.
*}
function ADictionary.add(const thisKey: string;
  const thisValue: Variant): ADictionaryEntry;

var
  // Refers to the new entry
  NewEntry: ADictionaryEntry;

begin
  NewEntry := ADictionaryEntry.named(thisKey, thisValue);

  if NewEntry <> nil then
  begin
    Result := ADictionaryEntry(Self.insertLeaf(NewEntry));

    if Result <> NewEntry then
    begin
      Result.setValue(thisValue);
      NewEntry.free;
    end;
  end;
end;

// Properties ------------------------------------------------------------------
{* Retrieves the value of the named item.

   \return The value of the named item, if it exists in the dictionary.  If the
   item is not found, the routine returns Null.
*}
function ADictionary.valueOf(const key: string): Variant;

var
  // Refers to the named item
  ThisEntry: ADictionaryEntry;

begin
  result := Null;
  ThisEntry := ADictionaryEntry(Self.fetchLeaf(key));

  if ThisEntry <> nil then
    result := ThisEntry.value;
end;

{* Retrieves the value of the named item.

   \return The value of the named item, if it exists in the dictionary.  If the
   item is not found, the routine returns \p defaultValue.
*}
function ADictionary.valueOf(const key: string;
  const defaultValue: variant): Variant;

var
  // Refers to the named item
  ThisEntry: ADictionaryEntry;

begin
  result := defaultValue;

  ThisEntry := ADictionaryEntry(Self.fetchLeaf(key));

  if ThisEntry <> nil then
    result := ThisEntry.value;
end;

{* Sets the value of the named item.  If the item does not yet exist in the
   dictionary, a new entry will be added (through a call to Dictionary.add()).
*}
function ADictionary.setValueOf(const key: string;
  const thisValue: Variant): Variant;

var
  // Refers to the named item
  ThisEntry: ADictionaryEntry;

begin
  Result := Variants.Null;

  ThisEntry := ADictionaryEntry(Self.fetchLeaf(key));
  if ThisEntry <> nil then
    result := ThisEntry.setValue(thisValue)

  else
    Self.add(key, thisValue);
end;

/// Retrieves the value at the specified offset
function ADictionary.valueAt(const thisIndex: longword): Variant;

var
  // Used to loop through the dictionary
  ThisItem: ADictionaryEntry;

begin
  result := Variants.null;

  ThisItem := ADictionaryEntry(Self.leafAtIndex(thisIndex));

  if ThisItem <> nil then
    result := ThisItem.value;
end;

/// Sets the value at the specified offset
function ADictionary.setValueAt(const thisIndex: longword;
  const newValue: variant): variant;

var
  // Used to loop through the dictionary
  ThisItem: ADictionaryEntry;

begin
  result := Variants.null;

  ThisItem := ADictionaryEntry(Self.leafAtIndex(thisIndex));

  if ThisItem <> nil then
    result := ThisItem.setValue(newValue);
end;
{$ENDIF}

