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
type ADictionaryEntry = class(AStringLeaf)
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

    { Construct a new entry from a string of the form
      @code(key[delimiter]value).  The key will be separated from the value
      based on the value of @code(keyValueDelimiter).  If
      @code(keyValueDelimiter) is not specified by the caller, or if it is an
      empty string, then the value provided by @link(dcteDefaultDelimiter) will
      be used.

      This method is useful when parsing a list of strings that separate the
      key from the value, such as might be found in a simple @code(.ini) file,
      where settings are specified using the form @code(key=value).
    }
    constructor from(thisDelimitedString: string;
      keyValueDelimiter: string = ''); virtual;

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

  // CanPrint implementation ---------------------------------------------------
    { Construct and return a string representation of the entry, suitable for
      printing to a text-based device, such as a console.

      The representation returned by this method will include the name of the
      entry (returned by a call to
      @link(AStringLeaf.name ADictionaryEntry.name)) and a string
      representation of its value (returned by a call to
      @code(Variants.varToStr)), separated by the delimiter specified in
      @link(dcteDefaultDelimiter).  The format of this string is controlled by
      @link(dcteStringRepresentation).
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieves the value associated with the entry. }
    function value: Variant; virtual;

    { Sets the value of the entry.

      @return(The previous value of the entry.)
    }
    function setValue(const newValue: Variant): Variant; virtual;
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
type ADictionary = class(ABinaryTree)
  public
    { Construct a new dictionary entrance that will parse the given string to
      populate its entries.

      @code(thisList) must be a doubly-delimited string: each key and value
      pair must be separated from the next by @code(pairsDelimiter), and the
      keys themselves must be separated from their associated values by
      @code(keyValueDelimiter).

      If @code(pairsDelimiter) is not specified by the caller, or if it is an
      empty string, then the value of @link(slstDefaultDelimiter) is used;
      likewise, if @code(keyValueDelimiter) is not specified by the caller, or
      if it is an empty string, then the value of @link(dcteDefaultDelimiter) is
      used.

      See @link(ADictionary.explode) for more information.
    }
    constructor from(const thisList: string;
      const pairsDelimiter: string = '';
      const keyValueDelimiter: string = '');

    { Initializer }
    function init: boolean; override;

    { Parses the given string to add new entries to the dictionary.

      @code(thisList) must be a doubly-delimited string: each key and value
      pair must be separated from the next by @code(pairsDelimiter), and the
      keys themselves must be separated from their associated values by
      @code(keyValueDelimiter).

      If @code(pairsDelimiter) is not specified by the caller, or if it is an
      empty string, then the value of @link(slstDefaultDelimiter) is used;
      likewise, if @code(keyValueDelimiter) is not specified by the caller, or
      if it is an empty string, then the value of @link(dcteDefaultDelimiter) is
      used.

      An example of a string that would be successfully parsed by this routine
      is shown below: @longCode(#
        const
          simpleList: string = 'firstItem=1,secondItem=second,thirdItem=42';
      #)

      The above string would result in three dictionary entries when passed to
      this routine: the first named @code(firstItem) with a value of @code(1);
      the second named @code(secondItem) with a value of @code(second); and the
      third named @code(thirdItem) with a value of @code(42).

      @return(The total number of items added to the dictionary.)
    }
    function explode(thisList: string;
      pairsDelimiter: string = ''; keyValueDelimiter: string = ''): longword;
      virtual;

    { Condenses the dictionary and its entries into a doubly-delimited string.
      In essence, this method reverses the work done by
      @link(ADictionary.explode); and, in fact, the string returned by this
      routine can be passed to @link(ADictionary.explode) to reconstitute the
      dictionary.

      The name of each dictionary entry is associated with its value and
      delimited by @code(keyValueDelimiter).  If @code(keyValueDelimiter) is
      not specified by the caller, or if it is an empty string, then the
      value of @link(dcteDefaultDelimiter) is used.

      The key and value pairs are separated from each other within the resulting
      string using the value of @code(pairsDelimiter).  If @code(pairsDelimiter)
      is not specified by the caller, or if it is an empty string, then value
      of @link(slstDefaultDelimiter) is used.

      @return(A doubly-delimited string that contains representations of all
      dictionary entries.)
    }
    function implode(pairsDelimiter: string = '';
      keyValueDelimiter: string = ''): string; virtual;

    { Constructs a list of all keys in the dictionary.

      This method proceeds sequentially through all entries in the dictionary,
      obtaining the name of each.  The names are inserted into an instance of
      @link(AStringList) which is constructed for this purpose.

      @return(An instance of @link(AStringList) that contains the names of all
      dictionary entries.  This list should be freed by the caller when it is
      no longer required.  If there are no entries in the dictionary, an empty
      string list is returned.)
    }
    function keys: AStringList; virtual;

    { Constructs a delimited string that contains the names of all entries in
      the dictionary.  The names are separated from each other by the string
      specified in @code(delimiter).  If @code(delimiter) is not provided by
      the caller, or if it is an empty string, then the value of
      @link(slstDefaultDelimiter) is used.
    }
    function implodeKeys(delimiter: string = ''): string; virtual;

    { Constructs a list of all entry values in the dictionary.

      This method proceeds sequentially through all entries in the dictionary,
      obtaining the value of each.  The values are inserted into an instance of
      @link(AVariantList) which is constructed for this purpose.

      @return(An instance of @link(AVariantList) that contains the values of
      all dictionary entries.  This list should be freed by the caller when it
      is no longer required.  If there are no entries in the dictionary, an
      empty variant list is returned.)
    }
    function values: AVariantList; virtual;

    { Constructs a delimited string that contains the values of all entries in
      the dictionary.  The values are separated from each other by the string
      specified in @code(delimiter).  If @code(delimiter) is not provided by
      the caller, or if it is an empty string, then the value of
      @link(slstDefaultDelimiter) is used.
    }
    function implodeValues(delimiter: string = ''): string; virtual;

    { Adds the specified key and value to the dictionary.

      As with other instances of @link(ABinaryTree), @link(ADictionary) does
      not allow duplicate entries; consequently, if @code(thisKey) already
      exists in the dictionary, its value will be updated with @code(thisValue).
      Otherwise, a new entry is created.

      @return(A reference to the dictionary entry that has been created and
      inserted or, if the entry already existed, the existing entry.)
    }
    function add(const thisKey: string;
      const thisValue: Variant): ADictionaryEntry; reintroduce; virtual;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the dictionary and its
      entries.

      This method simply calls @link(ADictionary.implode) with the default
      values for @code(pairsDelimiter) and @code(keyValueDelimiter).
    }
    function toString: AnsiString; override;

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

    { Retrieves the key at the specified index in the dictionary.

      Internally, this method calls @link(ABinaryTree.LeafAtIndex) on itself; if
      that method returns a node, then
      @link(AStringLeaf.name ADictionaryEntry.name) is called on it.

      @return(The name of the node at the specified @code(index), if found;
      an empty string otherwise.)
    }
    function keyAt(const thisIndex: longword): string; virtual;

    { Retrieves the value at the specified index in the dictionary.

      Internally, this method calls @link(ABinaryTree.LeafAtIndex) on itself; if
      that method returns a node, then @link(ADictionaryEntry.value) is called
      on it.

      @return(The value of the node at the specified @code(index), if found;
      @code(Variants.null) otherwise.)
    }
    function valueAt(const thisIndex: longword): Variant; virtual;

    { Set the value of the entry at the specified index in the dictionary.

      Internally, this method calls @link(ABinaryTree.LeafAtIndex) on itself; if
      that method returns a node, then @link(ADictionaryEntry.setValue) is
      called on it.

      @return(The previous value of the node at the specified @code(index), if
      found; @code(Variants.null) otherwise.)
    }
    function setValueAt(const thisIndex: longword;
      const newValue: Variant): Variant; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string is used as the default key/value pair delimiter when none is
    provided to @link(ADictionary.implode), @link(ADictionary.explode) and
    @link(ADictionaryEntry.from).
  }
  dcteDefaultDelimiter = '=';

  { This string controls the way that @link(ADictionaryEntry.toString)
    constructs a string representation of an instance of
    @link(ADictionaryEntry).

    The first string placeholder will be replaced by the name of the entry, as
    returned by a call to @link(AStringLeaf.name) when called on the entry.
    The second string placeholder will be filled with the value of
    @link(dcteDefaultDelimiter), used to separate the name of the entry from its
    value.  The final string placeholder will be filled with the value of the
    entry, as obtained by a call to @code(Variants.varToStr) on the entry value.
  }
  dcteStringRepresentation = '%s%s%s';
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

{* Constructs and initializes a new dictionary entry using the specified string.
   The string is assumed to contain the item key and value, separated by the
   delimiter character or characters specified by \p thisDelimiter.
   \p thisDelimiter is set as the value of ADictionaryEntry.delimiter.

   \return A new instance of the dictionary entry on success; #nil on failure.
*}
constructor ADictionaryEntry.from(thisDelimitedString: string;
  keyValueDelimiter: string = '');

begin
  if keyValueDelimiter = '' then
    keyValueDelimiter := dcteDefaultDelimiter;

  // Chomp the key from the string
  inherited named(Charstring.chomp(thisDelimitedString,
    keyValueDelimiter));
  // Whatever remains in the string is the value of the entry
  myValue := thisDelimitedString;
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

// CanPrint implementation -----------------------------------------------------
{* Creates a string representation of the dictionary item.  This representation
   will contain the key and value of the item separated by the delimiter
   specified by ADictionaryEntry.delimiter.

   \return A string representation of the entry, as described above.
*}
function ADictionaryEntry.toString: AnsiString;

begin
  result := SysUtils.format(dcteStringRepresentation, [
    myName, dcteDefaultDelimiter, Variants.varToStr(myValue)
  ]);
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

// ADictionary *****************************************************************
{* Constructs a new dictionary using the items in \p thisList.  \p thisList is
   assumed to be a delimited list of dictionary entries, where each item in the
   list is separated into a key and value pair.

   \return A new dictionary instance on success; #nil on failure.
*}
constructor ADictionary.from(const thisList: string;
  const pairsDelimiter: string = '';
  const keyValueDelimiter: string = '');

begin
  inherited new;

  Self.explode(thisList, pairsDelimiter, keyValueDelimiter);
end;

// Initializer
function ADictionary.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := ADictionaryEntry;
  end;
end;

{* Expands the specified list of items and adds them to the dictionary.
   \p thisList is assumed to be a delimited list of items, where each item is
   a key and value pair delimited by \p thisKeyValueDelimiter.

   \return The number of items added to the dictionary.  This value will be
   zero (0) if \p thisList is an empty string.
*}
function ADictionary.explode(thisList: string;
  pairsDelimiter: string = '';
  keyValueDelimiter: string = ''): longword;

var
  // A string list used to break up the list of items
  ItemsList: AStringList;
  // An item from the items list
  ThisItem: AStringListItem;
  // A new entry in the dictionary
  ThisEntry: ADictionaryEntry;

begin
  result := 0;

  if thisList <> '' then
  begin
    if pairsDelimiter = '' then
      pairsDelimiter := slstDefaultDelimiter;

    if keyValueDelimiter = '' then
      keyValueDelimiter := dcteDefaultDelimiter;

    // Break the list apart at the pairs delimiters
    ItemsList := AStringList.from(thisList, pairsDelimiter);

    if ItemsList <> nil then
    begin
      ThisItem := AStringListItem(ItemsList.FirstItem);
      // Traverse the list and add each entry
      while ThisItem <> nil do
      begin
        ThisEntry := ADictionaryEntry.from(ThisItem.text, keyValueDelimiter);

        if ThisEntry <> nil then
        begin
          // Check to ensure that insertion was successful
          if Self.insertLeaf(ThisEntry) = ThisEntry then
            inc(result)

          else
            // An unsuccessful entry is usually the result of a duplicate key
            ThisEntry.free;
        end;

        ThisItem := AStringListItem(ThisItem.Next);
      end;
    end;
  end;
end;

{* Condenses the dictionary and its entries into a string.  The string is
   constructed as a delimited list of values, where each value is a key/value
   pair.

   \return A string representation of the dictionary, as described above.
*}
function ADictionary.implode(pairsDelimiter: string = '';
  keyValueDelimiter: string = ''): string;

var
  // Refers to a list of entries constructed from the dictionary entries
  EntryList: AStringList;
  // Refers to an entry from the dictionary
  ThisEntry: ADictionaryEntry;

begin
  result := '';

  if MyChild <> nil then
  begin
    EntryList := AStringList.new;
    ThisEntry := ADictionaryEntry(MyRoot);

    while ThisEntry <> nil do
    begin
      EntryList.push(SysUtils.format(dcteStringRepresentation, [
        ThisEntry.name, keyValueDelimiter, Variants.varToStr(ThisEntry.value)
      ]));

      ThisEntry := ADictionaryEntry(ThisEntry.Next);
    end;

    // Now implode the string list
    result := EntryList.implode(pairsDelimiter);
    EntryList.free;
  end;
end;

{* Retrieves a list of dictionary keys.

   \return A list of dictionary keys.  If the dictionary contains no entries,
   this routine will return an empty list.
*}
function ADictionary.keys: AStringList;

var
  // Refers to an entry from the dictionary
  ThisEntry: ADictionaryEntry;

begin
  Result := AStringList.new;

  ThisEntry := ADictionaryEntry(MyRoot);

  while ThisEntry <> nil do
  begin
    Result.push(ThisEntry.name);

    ThisEntry := ADictionaryEntry(ThisEntry.Next);
  end;
end;

{* Condenses the list of dictionary keys into a string.  This routine is
   provided primarily for convenience, as it simply calls ADictionary.keys()
   and then calls AStringList.implode() on the resulting list.

   \return A string containing a list of all keys in the dictionary, delimited
   by the character or characters specified by \p thisDelimiter.  If there are
   no entries in the dictionary, this routine returns an empty string.
*}
function ADictionary.implodeKeys(delimiter: string = ''): string;

var
  // Refers to the list of keys
  KeyList: AStringList;

begin
  result := '';

  if delimiter = '' then
    delimiter := slstDefaultDelimiter;

  KeyList := Self.keys;

  if KeyList <> nil then
  begin
    result := KeyList.implode(delimiter);
    KeyList.free;
  end;
end;

{* Retrieves a list of dictionary values.  This routine calls
   ADictionaryEntry.implodeValueInto() on its root node in order to obtain a
   list of every dictionary value.

   \return A list of dictionary values.  If the dictionary contains no entries,
   this routine will return #nil.
*}
function ADictionary.values: AVariantList;

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

{* Condenses the list of dictionary values into a string.  This routine is
   provided primarily for convenience, as it simply calls ADictionary.values()
   and then calls AStringList.implode() on the resulting list.

   \return A string containing a list of all values in the dictionary, delimited
   by the character or characters specified by \p thisDelimiter.  If there are
   no entries in the dictionary, this routine returns an empty string.
*}
function ADictionary.implodeValues(delimiter: string = ''): string;

var
  // Refers to the list of values
  ValueList: AVariantList;

begin
  result := '';

  if delimiter = '' then
    delimiter := slstDefaultDelimiter;

  ValueList := Self.Values;

  if ValueList <> nil then
  begin
    result := ValueList.implode(delimiter);
    ValueList.free;
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

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the dictionary.  This routine first
   calls ADictionary.implode() to obtain a string representation of the
   dictionary and its entries.

   \return A string representation of the dictionary, as described above.
*}
function ADictionary.toString: AnsiString;

begin
  result := Self.implode;
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

/// Retrieves the key at the specified offset
function ADictionary.keyAt(const thisIndex: longword): string;

var
  // Used to loop through the dictionary
  ThisItem: ADictionaryEntry;

begin
  result := '';

  ThisItem := ADictionaryEntry(Self.leafAtIndex(thisIndex));

  if ThisItem <> nil then
    result := ThisItem.name;
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

