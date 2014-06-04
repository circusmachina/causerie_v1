{ binarytrees.inc: Binary nodes and the trees to manage them.

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
{ This type is used to represent a sort key, which is used to arrange items in
  a binary tree or list.
}
type TSortKey = int64;

// ABinaryLeaf *****************************************************************
type
  { This type is used to find and indicate the position at which a new node
    should be inserted into the tree.
  }
  ABinaryLeafPointer = ^ABinaryLeaf;

  { This class represents a node in a binary tree.  The node is sorted based on
    the value of @link(ABinaryLeaf.sortKey), which can be an integer value
    (negative or positive) or a string value.  In the case of string values, the
    hash value of the string (calculated by a call to
    @link(Charstring.hashValueOf) is used to determine the value of the sort key.

    This class is declared as a descendant of @link(ANode) to allow iteration
    through binary tree nodes.
  }
  ABinaryLeaf = class(ANode)
  protected
    { Refers to the left subtree of the node. }
    MyLeftTree: ABinaryLeaf;
    { Refers to the right subtree of the node. }
    MyRightTree: ABinaryLeaf;
    { Stores the key used to sort the node. }
    mySortKey: TSortKey;
    { Stores the index of the node }
    myIndex: longword;

  public
    { Construct a new node that uses the specified value as its sort key. }
    constructor withSortKey(const thisKey: TSortKey); virtual;

    { Construct a new node that uses the hash value of the specified string as
      its sort key.
    }
    constructor named(const thisKey: string); virtual;

    { Initializer }
    function init: boolean; override;

    { Determine whether or not the specified sort key is contained within the
      node or one of its subtrees.

      This method first checks to see whether @code(thisKey) matches the sort
      key of the current node; if so, it returns @true immediately.  Otherwise,
      the method performs a recursive search of either the left or right
      subtree of the node, depending upon whether the value of @code(thisKey) is
      greater than or less than the sort key of the current node.

      The recursive nature of the search means that the entire tree is searched
      if this method is called on the root node of a binary tree.

      @return(@true if @code(thisKey) is found within the node or one of its
      subtrees; @false if not.)
    }
    function hasKey(const thisKey: TSortKey): boolean; virtual;

    { Determine whether or not a sort key with the same value as the hash
      value of @code(thisKey) is contained within the node or its subtrees.

      This method takes the hash value of @code(thisKey), as calculated by a
      call to @link(Charstring.hashValueOf), and then calls the integer version
      of @link(ABinaryLeaf.hasKey) on @code(Self).

      @return(@true if a sort key that matches the hash value of @code(thisKey)
      is found within the node or one of its subtrees; @false if not.)
    }
    function hasKey(const thisKey: string): boolean; virtual;

    { Retrieves a reference to the node that has the specified sort key.

      This method first checks to see whether @code(thisKey) matches the sort
      key of the current node; if so, it returns @code(Self).  Otherwise, the
      method performs a recursive search of either the left or right subtree
      of the node, depending upon whether the value of @code(thisKey) is
      greater than or less than the sort key of the current node.

      The recursive nature of the search means that the entire tree is searched
      if this method is called on the root node of a binary tree.

      @return(A reference to the node that contains the specified sort key, if
      found within the node or one of its subtrees; @nil otherwise.)
    }
    function fetch(const thisKey: TSortKey): ABinaryLeaf; virtual;

    { Retrieves a reference to the node that has a sort key which matches the
      hash value of the specified string.

      This method takes the hash value of @code(thisKey), as calculated by a
      call to @link(Charstring.hashValue), and then calls the integer version
      of @link(ABinaryLeaf.fetch) on @code(Self).

      @return(A reference to the node that contains a sort key which matches the
      hash value of @code(thisKey), if found within the node or one of its
      subtrees; @nil otherwise.)
    }
    function fetch(const thisKey: string): ABinaryLeaf; virtual;

    { Inserts @code(ThisLeaf) into one of the subtrees of the current node.

      This method compares the sort key of @code(ThisLeaf) with its own sort
      key.  If the sort key of @code(ThisLeaf) is greater than the sort key
      of the current leaf, then @code(ThisLeaf) is inserted into the right
      subtree of the current node.  If the sort key of @code(ThisLeaf) is less
      than the sort key of the current leaf, then @code(ThisLeaf) is inserted
      into the left subtree of the current node.  If the values match, then
      this routine returns a reference to @code(Self) and NO INSERTION OCCURS;
      this behavior effectively prevents duplicate sort keys from existing
      within the same tree.

      If this method is called on the root node of a binary tree, it will
      ensure that the new leaf is inserted at the appropriate position within
      the tree.

      @return(A reference to @code(ThisLeaf) if the new node was successfully
      inserted.  If a node already exists with the same sort key as
      @code(ThisLeaf), then a reference to that node is returned.  To determine
      whether the call successfully inserted @code(ThisLeaf), the calling
      routine should check to see whether the return value of this function
      points to the same location as @code(ThisLeaf).)
    }
    function insert(const ThisLeaf: ABinaryLeaf): ABinaryLeaf; virtual;

    { Determine the insertion point of a node that has the specified sort key.

      This method compares the sort key of the current node against
      @code(thisKey).  If the sort key of the current node is less than
      @code(thisKey) and @link(ABinaryLeaf.RightTree) is @nil when called on
      @code(Self) OR the sort key of the node referred to by the right
      subtree matches @code(thisKey), then a pointer to that node is returned.

      Similarly, if the sort key of the current node is greater than
      @code(thisKey) and @link(ABinaryLeaf.LeftTree) is @nil when called on
      @code(Self) OR the sort key of the node referred to by the left subtree
      matches @code(thisKey), then a pointer to that node is returned.

      Otherwise, the method is called recursively on either the left or right
      subtree until a match or @nil subtree reference is found.  If this
      method is called on the only node in a tree, @nil will be returned.

      The fact that a pointer to @code(ABinaryLeaf) is returned allows the
      caller to directly manipulate the point at which a node containing
      @code(thisKey) is inserted into the overall tree.  This behavior is
      used by @link(ABinaryTree.remove) to unlink a given node from its
      position within the tree.
    }
    function insertionPointOf(const thisKey: TSortKey): ABinaryLeafPointer;
      virtual;

    { Determine the insertion point of a node that has a sort key which matches
      the hash value of @code(thisKey).

      This method takes the hash value of @code(thisKey), as calculated by a
      call to @link(Charstring.hashValue), and then calls the integer version of
      @link(ABinaryLeaf.insertionPointOf).
    }
    function insertionPointOf(const thisKey: string): ABinaryLeafPointer;
      virtual;

    { Detach the node from its associated tree.

      This method does little more than nullify the references to the left
      and right subtrees of the node before calling the inherited routine.
      It is called by @link(ABinaryTree.remove) as part of the process involved
      in removing a specific node from the overall tree.
    }
    procedure detach; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(ABinaryLeaf).  If so,
      it copies the values of
      @unorderedList(
        @item(@link(ABinaryLeaf.key) and)
        @item(@link(ABinaryLeaf.index))
      )
      from @code(Other) to @code(Self), overwriting any values in @code(Self).

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

    { Write the node, and just the node, to the specified stream.  For more
      information on this routine, see @link(ANode.writeSelfTo).

      This method overrides the behavior inherited from @link(ANode.writeTo);
      instead of writing the number of children of the node, it writes the
      sort key and index of the node.  It is assumed that instances of
      @link(ABinaryLeaf) and its descendants will not have child nodes.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the node from the specified stream.

      This method overrides the behavior inherited from @link(ANode.readFrom);
      instead of attempting to read all children of the node, it simply reads
      the sort key and index of the node.  It is assumed that instances of
      @link(ABinaryLeaf) and its descendants will have no child nodes.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the node, suitable for
      printing to a text-based console or device.

      The base implementation of this method within @link(ABinaryLeaf) returns
      the class name of the node and the value of its key and index.  The
      format of the string returned is controlled by
      @link(bnlfStringRepresentation).
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieve a reference to the first node in the left subtree which branches
      from this node.

      As implemented within @link(ABinaryLeaf), the left subtree contains all
      nodes with values that are LESS than that of @code(Self), as determined
      by a call to @link(CanCompare.isLessThan).

      If there are no nodes in the left subtree, this routine will return
      @nil.
    }
    function LeftTree: ABinaryLeaf; virtual;

    { Retrieve a reference to the first node in the right subtree which branches
      from this node.

      As implemented within @link(ABinaryLeaf), the right subtree contains all
      nodes with values that are GREATER than that of @code(Self), as
      determined by a call to @link(CanCompare.isGreaterThan).

      If there are no nodes in the right subtree, this routine will return
      @nil.
    }
    function RightTree: ABinaryLeaf; virtual;

    { Retrieve the value of the sort key associated with this node.  This is
      the value that is used in calls to @link(ABinaryLeaf.insert) and others,
      in order to determine where the node is placed in the tree to which it
      belongs.
    }
    function sortKey: TSortKey; virtual;

    { Retrieve the present index of the node.  This value represents the
      sequence in which the node was entered into its parent tree and may
      change if nodes are removed from the tree.

      The primary purpose of this value is to use when serializing the parent
      tree to which the node belongs, usually for storage in a stream and
      later recall by an instance of @link(ASerializedList).  One scenario
      where this behavior would be used is in the creation and storage of a
      symbol table.
    }
    function index: longword; virtual;
end;

// ABinaryTree *****************************************************************
/// A binary tree
{  This class represents a basic binary tree which contains nodes that are
   sorted by an integer key.  The tree can manage ABinaryLeaf and its
   descendants.  Binary trees are declared as instances of @link(ABinaryLeaf)
   so that they can be themselves entered into binary trees and managed as
   lists of binary trees.
}
type
  ABinaryTree = class(ABinaryLeaf, CanIterate)
  protected
    { Indicates the type of binary leaf managed by the tree }
    MyLeafType: ANodeClass;
    { Refers to the oldest node in the tree }
    MyRoot: ABinaryLeaf;

  public
    { Initializer }
    function init: boolean; override;

    { Searches the root node and its subtrees for the leaf which has the sort
      key that matches the one specified by @code(thisKey).

      This method operates only on the current tree.  It simply calls
      @link(ABinaryLeaf.fetch) on its root node and returns the result.

      @return(A reference to the node with the sort key that matches
      @code(thisKey), if found; @nil otherwise.  This routine will also return
      @nil if the tree has no nodes.)
    }
    function fetchLeaf(const thisKey: TSortKey): ABinaryLeaf; virtual;

    { Searches the root node and its subtrees for the leaf which has a sort key
      that matches the hash value of @code(thisKey).

      This method operates only on the current tree.  It simply calls
      @link(ABinaryLeaf.fetch) on its root node and returns the result.

      @return(A reference to the node with the sort key that has a sort key
      which matches the hash value of @code(thisKey), if found; @nil otherwise.
      This method will also return @nil if the tree has no nodes.)
    }
    function fetchLeaf(const thisKey: string): ABinaryLeaf; virtual;

    { Retrieves the node at the specified index.

      Unlike instances of @link(ALinkedList), @code(index) must be an absolute
      offset -- that is, an offset relative to the root node of the tree.  The
      root node will always be at index zero (0).  If @code(index) specifies a
      value that is greater than or equal to @link(ABinaryTree.census), then
      the last node in the tree is returned.

      This method searches sequentially through the nodes in the tree and so
      may be inefficient for large trees.

      @return(A reference to the node with the specified index, if found;
      @nil otherwise).
    }
    function leafAtIndex(thisindex: longword): ABinaryLeaf; virtual;

    { Determine whether or not the root node or its subtrees contain a node
      with a sort key that matches @code(thisKey).

      This method operates only on the current tree.  It simply calls
      @link(ABinaryLeaf.hasKey) on its root node and returns the result.

      @return(@true if a node is found that has a sort key which matches the
      value of @code(thisKey); @false if not.  This method will also return
      @false if the tree is empty.)
    }
    function hasLeafWithKey(const thisKey: TSortKey): boolean; virtual;

    { Determine whether or not the root node or its subtrees contain a node with
      a sort key that matches the hash value of @code(thisKey).

      This method operates only on the current tree.  It simply calls
      @link(ABinaryLeaf.hasKey) on its root node and returns the result.

      @return(@true if a node is found that has a sort key which matches the
      hash value of @code(thisKey); @false if not.  This method will also return
      @false if the tree is empty.)
    }
    function hasLeafWithKey(const thisKey: string): boolean; virtual;

    { Inserts @code(Leaf) into the tree, provided that a node with the sort
      key specified by @code(Leaf) does not already exist therein.

      This method simply calls @link(ABinaryLeaf.insert) on the root node of the
      tree and returns the result.  If the tree does not yet have a root node,
      then @code(Leaf) becomes the root node.

      To determine whether the insertion was successful, the caller can compare
      the reference returned by this routine with that of @code(Leaf) itself;
      if the two match, then the insertion was successful; if not, then it
      failed.  For example: @longcode(#

        var
          MyNode: ABinaryLeaf;

        begin
          // Construct the node
          MyNode := ABinaryLeaf.withKey(42);

          // We assume that 'MyTree' has already been constructed somewhere
          if MyTree.insertLeaf(MyNode) = MyNode then
          begin
            // Insertion succeeded
          end

          else begin
            // Insertion failed, most likely because '42' is already in the tree
            MyNode.free;
          end;

      #)

      Alternatively, you can call @link(ABinaryTree.add), which will return
      @true or @false to indicate whether the new node was successfully
      inserted.

      If the insertion succeeded, the tree assumes control of the node and
      @bold(the caller should not attempt to free it), as this will cause the
      program to crash when the tree is freed.

      @return(A reference to @code(Leaf), if the insertion succeeded.  If a
      node with the same sort key as @code(Leaf) already exists in the tree,
      then a reference to that node is returned.)
    }
    function insertLeaf(const Leaf: ABinaryLeaf): ABinaryLeaf; virtual;

    { Add a new node to the tree, provided that a node with sort key specified
      by @code(Leaf) does not already exist in the tree.

      This method simply calls @link(ABinaryTree.insertLeaf) to insert
      @code(Leaf) into the tree.  It then checks to see whether the return value
      of that call matches @code(Leaf); if so, the new node was inserted
      successfully.  Otherwise, the insertion failed.

      If @code(freeOnFailure) is @true, then the new node will be destroyed by
      this routine if the insertion fails.  This behavior is designed to allow
      this method to be called with a node constructor as the leaf to be
      inserted:  @longcode(#
          if MyTree.addLeaf(ABinaryLeaf.withKey(42), true) then ...
      #)

      @return(@true if @code(Leaf) was successfully inserted into the tree;
      @false if not.)
    }
    function addLeaf(const Leaf: ABinaryLeaf;
      const freeOnFailure: boolean = false): boolean; virtual;

    { Removes, but does not free, the node with the specified key from the
      tree.

      This method searches the root node and its subtrees for a node with a
      sort key that matches @code(thisKey); if the node is found, it is detached
      from its subtrees and returned.  The subtrees are reinserted into the
      tree.

      Use this method with care!  Because @link(ABinaryTree) relies on the
      current value of @link(ANode.census) to determine the index of new nodes
      that are inserted into the tree, removing and then adding nodes could
      result in nodes with duplicate indices in the same tree.  If you rely on
      indices being constant, you should call ensure that
      @code(updateIndices) is @true, so that @link(ABinaryTree.reindex) is
      called after the node is deleted.

      @return(A reference to the node with a sort key that matches the one
      specified, if found; @nil otherwise.  If the node is found, the caller
      is given responsibility for freeing it, as it will no longer be part of
      the tree.)
    }
    function removeLeaf(const thisKey: TSortKey;
      const updateIndices: boolean = true): ABinaryLeaf; virtual;

    { Removes from the tree, but does not free, the node with a key that
      matches the hash value of @code(thisKey).

      This method searches the root node and its subtrees for a node with a
      sort key that matches the hash value of @code(thisKey); if the node is
      found, it is detached from its subtrees and returned.  The subtrees are
      reinserted into the tree.

      Use this method with care!  Because @link(ABinaryTree) relies on the
      current value of @link(ANode.census) to determine the index of new nodes
      that are inserted into the tree, removing and then adding nodes could
      result in nodes with duplicate indices in the same tree.  If you rely on
      indices being constant, you should call ensure that
      @code(updateIndices) is @true, so that @link(ABinaryTree.reindex) is
      called after the node is deleted.

      @return(A reference to the node with a sort key that matches the one
      specified, if found; @nil otherwise.  If the node is found, the caller
      is given responsibility for freeing it, as it will no longer be part of
      the tree.)
    }
    function removeLeaf(const thisKey: string;
      const updateIndices: boolean = true): ABinaryLeaf; virtual;

    { Deletes the node with the specified key from the tree.

      This method calls @link(ABinaryTree.remove) to find the node that has
      @code(thisKey).  If it is found, then the node is freed.

      Use this method with care!  Because @link(ABinaryTree) relies on the
      current value of @link(ANode.census) to determine the index of new nodes
      that are inserted into the tree, removing and then adding nodes could
      result in nodes with duplicate indices in the same tree.  If you rely on
      indices being constant, you should call ensure that
      @code(updateIndices) is @true, so that @link(ABinaryTree.reindex) is
      called after the node is deleted.

      @return(@true if the node was found and freed; @false otherwise.)
    }
    function deleteLeaf(const thisKey: TSortKey;
      const updateIndices: boolean = true): boolean; virtual;

    { Deletes the node with a key that matches the hash value of @code(thisKey).

      This method calls @link(ABinaryTree.remove) to find the node that has
      a hash value which matches @code(thisKey).  If it is found, then the node
      is freed.

      Use this method with care!  Because @link(ABinaryTree) relies on the
      current value of @link(ANode.census) to determine the index of new nodes
      that are inserted into the tree, removing and then adding nodes could
      result in nodes with duplicate indices in the same tree.  If you rely on
      indices being constant, you should call ensure that
      @code(updateIndices) is @true, so that @link(ABinaryTree.reindex) is
      called after the node is deleted.

      @return(@true if the node was found and freed; @false otherwise.)
    }
    function deleteLeaf(const thisKey: string;
      const updateIndices: boolean = true): boolean; virtual;

    { Reindex the tree.

      This method loops through each leaf in the tree, beginning with the root
      node, and assigns it an index starting from zero (0).  When the loop
      completes, the value of @link(ANode.census) as applied to the tree is
      updated to contain the most current count of the nodes in the tree.

      You will only need to call this method if you have removed nodes from the
      tree, but you expect each node index to be unique.  Because instances of
      @link(ABinaryTree) rely on the current value of @link(ANode.census) when
      assigning indices to new nodes, it is possible that a new node inserted
      into the tree, after one has been removed, will have the same index as
      another node in the tree.  This method will resolve such a scenario;
      however, since it proceeds sequentially through the tree, it can be
      inefficient for very large trees.
    }
    procedure reindex; virtual;

    { Construct a shallow copy of the other item.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(ABinaryTree).  If so, it
      constructs a copy of the other tree by calling
      @link(ABinaryLeaf.shallowCopyFrom) on each node.  The copied nodes are
      then inserted into the tree.

      Note that the behavior of this method allows a copy of a tree to be added
      to an existing tree, so long as the nodes to be copied do not have sort
      keys that are identical to those in the existing tree.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Write the tree and its nodes to the specified stream.

      This method overrides the behavior inherited from @link(ABinaryLeaf).
      It first writes the number of nodes in the tree, then calls
      @link(ABinaryLeaf.writeTo) on its root node, which causes all nodes in
      the tree to be written to the stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    { Print the tree and its nodes to the specified stream.

      This method overrides the behavior inherited from @link(ANode).  It
      first writes a string representation of itself, as obtained by calling
      @link(ABinaryTree.toString), then writes the value of @code(suffix).
      After that, it calls @link(ABinaryLeaf.printTo) on its root node, which
      causes all other nodes in the tree to be printed.

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printSelfTo(const Dest: AStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Reads the tree and its nodes from the specified stream.

      This method overrides the behavior inherited from @link(ABinaryLeaf).  It
      first reads the number of nodes from the stream, then iterates through a
      loop which constructs a new instance of @link(ABinaryTree.LeafType) for
      each node, before calling  @link(ABinaryLeaf.readFrom) on that instance
      and inserting the result into the tree.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Constructs and returns a string representation of the tree, suitable for
      printing to a display or text-based device.

      The representation returned will contain the name of the class and the
      number of nodes presently in the tree.  This representation is controlled
      by @link(bntrStringRepresentationSingular) and
      @link(bntrStringRepresentationPlural), depending on the number of nodes
      presently in the tree.
    }
    function toString: AnsiString; override;

    { Prints a string representation of the tree and its nodes to the specified
      stream.

      This method overrides the behavior inherited from @link(ANode): it first
      calls @link(ABinaryTree.printTo) on its left subtree, if any; then
      prints the string representation returned by a call to
      @link(ABinaryTree.toString) and calls @link(ABinaryLeaf.printTo) on
      its root node to recursively print a representation of every node in
      the tree.  Finally, it calls @link(ABinaryTree.printTo) on its right
      subtree, if any.

      @code(prefix) is not used by this routine, but it is passed to the root
      node and can be used there to specify a delimiter for each node that is
      printed.  If @code(prefix) is not specified, then this routine passes
      @link(bntrDefaultNodePrintingPrefix) to the root node.

      If @code(suffix) is not otherwise specified, this routine uses the value
      of @link(bntrDefaultPrintingSuffix) as its own suffix and passes the
      value of @link(bntrDefaultNodePrintingSuffix) to the root node.

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printTo(const Dest: AStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

    // CanIterate implementation -----------------------------------------------
    { Constructs and returns an iterator that is suitable for iterating through
      the nodes in the tree.

      For more information on this routine, see @link(CanIterate.iterator).

      As implemented in @link(ABinaryTree), this method returns an instance of
      @link(ABinaryTreeIterator).  Descendant classes may override this method
      to return a different class instance instead.
    }
    function Iterator: AnIterator; virtual;

  // Properties ----------------------------------------------------------------
    { Retrieve the the type of node that is managed by the tree.  This will be
      a reference to @link(ABinaryLeaf) or one of its descendants.
    }
    function LeafType: ANodeClass; virtual;

    { Retrieve a reference to the root node of the tree.  This is the node that
      was first inserted into the tree.

      The reference returned by this method should NOT be freed by the caller.

      If the tree has no nodes, this routine will return @nil.
    }
    function Root: ABinaryLeaf; virtual;

    { Retrieve a reference to the youngest node in the tree.  This is the node
      that was most recently added to the tree.

      The reference returned by this method should NOT be freed by the caller.

      If the tree has no nodes, this routine will return @nil.
    }
    function YoungestNode: ABinaryLeaf; virtual;
end;

// ABinaryTreeDictionary *******************************************************
{ This class represents a "dictionary" of binary trees, which is simply a
  binary tree of binary trees.  There may be occasions where it is desirable
  to group several binary trees together and this class makes such a thing
  possible.
}
type ABinaryTreeDictionary = class(ABinaryTree)
  public
    { Initializer }
    function init: boolean; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the way a representation of @link(ABinaryLeaf) is
    constructed by @link(ABinaryLeaf.toString).

    The string placeholder will be filled with the display name of the class, as
    returned by a call to @link(APrintingObject.displayName).  The two integer
    placeholders will be filled by the index and sort keys of the node,
    respectively.
  }
  bnlfStringRepresentation = '%s at index %d with key %d';

  { This string controls the way a representation of @link(ABinaryTree) is
    constructed by @link(ABinaryTree.toString) when the tree has exactly one
    node.

    The string placeholder will be filled with the display name of the class, as
    returned by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled by the value of @link(ABinaryTree.census), which
    will always be one (1) when this string is used.
  }
  bntrStringRepresentationSingular = '%s that contains %d node';

  { This string controls the way a representation of @link(ABinaryTree) is
    constructed by @link(ABinaryTree.toString) when the tree does not have
    exactly one node.

    The string placeholder will be filled with the display name of the class, as
    returned by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled by the value of @link(ABinaryTree.census).
  }
  bntrStringRepresentationPlural = '%s that contains %d nodes';

  { This string is used as the default prefix when @link(ABinaryTree.printTo)
    causes its nodes to be printed to a stream.  It is, by default, a tab
    character, which ensures that the printed descriptions of the tree's nodes
    are indented below the printed description of the tree itself.
  }
  bntrDefaultNodePrintingPrefix = #9;

  { This string is used as the default suffix when @link(ABinaryTree.printTo)
    causes its nodes to be printed to a stream.  It is, by default, the value
    of @code(System.lineEnding), which ensures that the printed description of
    each node occupies its own line.
  }
  bntrDefaultNodePrintingSuffix = lineEnding;

  { This string is used as the default suffix when @link(ABinaryTree.printTo)
    prints a description of the tree to a given stream.  It is, by default, the
    value of @code(System.lineEnding), which ensures that the printed
    description of the tree occupies a line by itself.  When used in conjunction
    with @link(bntrDefaultNodePrintingPrefix) and
    @link(bntrDefaultNodePrintingSuffix), this ensures that the printed
    description of each node is indented on its own line beneath the printed
    description of the tree.
  }
  bntrDefaultPrintingSuffix = lineEnding;
{$ENDIF}
{$IFDEF ReadImplementation}
// ABinaryLeaf *****************************************************************
{* Constructs a new instance of ABinaryLeaf using \p thisKey as the sort key for
   the node.

   \return A new instance of ABinaryLeaf on success; #nil on failure.
*}
constructor ABinaryLeaf.withSortKey(const thisKey: TSortKey);

begin
  inherited new;
  mySortKey := thisKey;
end;

// Construct a new instance using the hash value of the given string
constructor ABinaryLeaf.named(const thisKey: string);

begin
  inherited new;

  mySortKey := Charstring.hashValueOf(thisKey);
end;

{* Initializes a new instance of ABinaryLeaf.  All properties of the node are
   set to #nil or zero (0).  The routine then calls the inherited method (from
   AnObject), which means that, in practice, it always returns #true.

   \return #true if initialization was successful; #false if not.  In the base
   implementation, this routine always returns #true.
*}
function ABinaryLeaf.init: boolean;

begin
  MyLeftTree := nil;
  MyRightTree := nil;
  mySortKey := 0;
  myIndex := 0;
  result := inherited init;
end;

/// Determines whether the node or its subtrees has a node with the specified key
function ABinaryLeaf.hasKey(const thisKey: TSortKey): boolean;

begin
  result := false;

  if (thisKey < mySortKey) and (MyLeftTree <> nil) then
    result := MyLeftTree.hasKey(thisKey)

  else if (thisKey > mySortKey) and (MyRightTree <> nil) then
    result := MyRightTree.hasKey(thisKey)

  else if thisKey = mySortKey then
    result := true;
end;

// Determine whether the node has a node with the specified key
function ABinaryLeaf.hasKey(const thisKey: string): boolean;

begin
  result := Self.hasKey(Charstring.hashValueOf(thisKey));
end;

{* Fetches the node with the specified key from this node or one of its
   subtrees.  The value of \p thisKey is checked against the sort key of the
   present leaf; if the two match, then a pointer to the current leaf is
   returned.  Otherwise, the left or right subtrees are searched, depending on
   whether \p thisKey is lower or higher than the sort key of the current leaf.

   \return A reference to the leaf that contains \p thisKey, if found;
   otherwise, \p nil.
*}
function ABinaryLeaf.fetch(const thisKey: TSortKey): ABinaryLeaf;

var
  // Stores the relationship between the leaf in question and the specified key
  rel: int64;

begin
  Result := nil;

  // Compare the desired key against the one held by this leaf
  rel := thisKey - mySortKey;

  // Search the left or right subtrees, if needed
  if rel < 0 then
  begin
    if MyLeftTree <> nil then
      Result := MyLeftTree.fetch(thisKey);
  end

  else if rel > 0 then
  begin
    if myRightTree <> nil then
      Result := MyRightTree.fetch(thisKey);
  end

  else if rel = 0 then
    Result := Self;
end;

// Fetch the node with the specified key
function ABinaryLeaf.fetch(const thisKey: string): ABinaryLeaf;

begin
  Result := Self.fetch(Charstring.hashValueOf(thisKey));
end;

{* Inserts \p ThisLeaf into one of the subtrees attached to the node, but only
   if ABinaryLeaf::key(), when called on \p ThisLeaf, does not return a value
   that is already in one of the subtrees.

   \return \p ThisLeaf on success.  If a node already existed with the same
   sort key as \p ThisLeaf, then that node is returned.  To determine whether
   or not the insertion was successful, check to see whether the pointer
   returned by this function refers to the same address as \p ThisLeaf.
*}
function ABinaryLeaf.insert(const ThisLeaf: ABinaryLeaf): ABinaryLeaf;

var
  // Stores the relationship between the leaf in question and the specified key
  rel: int64;

begin
  Result := ThisLeaf;

  if ThisLeaf = nil then
    exit;

  // Compare the desired key against the one held by this leaf
  rel := ThisLeaf.mySortKey - mySortKey;

  // Search the left or right subtrees, if needed
  if rel < 0 then
  begin
    if MyLeftTree <> nil then
      Result := MyLeftTree.insert(ThisLeaf)

    else
      MyLeftTree := ThisLeaf;
  end

  else if rel > 0 then
  begin
    if myRightTree <> nil then
      Result := MyRightTree.insert(ThisLeaf)

    else
      MyRightTree := ThisLeaf;
  end

  else if rel = 0 then
    Result := Self;
end;

/// Finds the insertion point of the specified key
function ABinaryLeaf.insertionPointOf(const thisKey: TSortKey): ABinaryLeafPointer;

var
  // Stores the relationship between the leaf in question and the specified key
  rel: int64;

begin
  Result := nil;

  // Compare the desired key against the one held by this leaf
  rel := thisKey - mySortKey;

  // Search the left or right subtrees, if needed
  if rel < 0 then
  begin
    if MyLeftTree <> nil then
      Result := MyLeftTree.insertionPointOf(thisKey)

    else
      Result := @MyLeftTree;
  end

  else if rel > 0 then
  begin
    if myRightTree <> nil then
      Result := MyRightTree.insertionPointOf(thisKey)

    else
      Result := @MyRightTree;
  end

  else if rel = 0 then
    Result := @Self;
end;

// Find the insertion point of the specified key
function ABinaryLeaf.insertionPointOf(const thisKey: string): ABinaryLeafPointer;

begin
  Result := Self.insertionPointOf(Charstring.hashValueOf(thisKey));
end;

{* Detaches the leaf from its subtrees.  This routine is meant to be used on
   those occasions where it is necessary to remove a single node from the tree.
   Note that this routine does NOT re-insert the detached subtrees back into
   the hierarchy.
*}
procedure ABinaryLeaf.detach;

begin
  MyLeftTree := nil;
  MyRightTree := nil;
  inherited detach;
end;

// Construct a shallow copy of the given object
function ABinaryLeaf.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is ABinaryLeaf) then
    result := false;

  if result then
  begin
    mySortKey := ABinaryLeaf(Other).sortKey;
    myIndex := ABinaryLeaf(Other).index;
  end;
end;

{* Streams data from the node to \p Dest.  This routine is designed to be
   called from ABinaryLeaf.writeTo, which executes the call for each node
   connected to the specified leaf.

   In the base implementation, this routine writes the index of the node
   (a number that indicates the order in which the node was created) and
   the hash key of the node to ]p Dest.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ABinaryLeaf.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;
  if Dest <> nil then
  begin
    // Write our index
    result := Dest.write(sizeof(myIndex), myIndex);
    // Write our key
    result := result + Dest.write(sizeof(mySortKey), mySortKey);
  end;
end;

// CanStream implementation ----------------------------------------------------
{* Reads leaf data from \p Source.  In the base implementation, this function
   reads the index of the node and its sort key from \p Source.

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function ABinaryLeaf.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    // Read our index
    result := Source.read(sizeof(myIndex), myIndex);
    // Read our key
    result := result + Source.read(sizeof(mySortKey), mySortKey);
  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the leaf.  The format of the string
   is as follows: the name of the leaf, its index, and its sort key.  These

   \return A string representation of the leaf, as described above.
*}
function ABinaryLeaf.toString: string;

begin
  result := format(bnlfStringRepresentation, [
    Self.displayName, myIndex, mySortKey
  ]);
end;

// Properties ------------------------------------------------------------------
/// Retrieve the left subtree of the node
function ABinaryLeaf.LeftTree: ABinaryLeaf;

begin
  Result := MyLeftTree;
end;

/// Retrieve the right subtree of the node
function ABinaryLeaf.RightTree: ABinaryLeaf;

begin
  Result := MyRightTree;
end;

/// Retrieve the key associated with the node
function ABinaryLeaf.sortKey: TSortKey;

begin
  result := mySortKey;
end;

/// Retrieve the index of the node
function ABinaryLeaf.index: longword;

begin
  result := myIndex;
end;

// ABinaryTree *****************************************************************
{* Initializes a new instance of ABinaryTree.  This routine sets
   ABinaryTree.root to #nil and ABinaryTree.census to zero (0) before calling
   the inherited routine (AnObject.init).  As a result, in its default
   implementation, this routine will always return #true.

   \return #true if initialization was successful; #false if not.
*}
function ABinaryTree.init: boolean;

begin
  MyLeafType := ABinaryLeaf;
  MyRoot := nil;
  result := inherited init;
end;

// Fetch the specified node from the tree
function ABinaryTree.fetchLeaf(const thisKey: TSortKey): ABinaryLeaf;

begin
  Result := nil;

  if MyRoot <> nil then
    Result := MyRoot.fetch(thisKey);
end;

// Fetch the specified node from the tree
function ABinaryTree.fetchLeaf(const thisKey: string): ABinaryLeaf;

begin
  Result := Self.fetchLeaf(Charstring.hashValueOf(thisKey));
end;

// Retrieve the node with the specified index
function ABinaryTree.leafAtIndex(thisIndex: longword): ABinaryLeaf;

begin
  if thisIndex >= myChildCount then
    thisIndex := myChildCount - 1;

  Result := MyRoot;

  while Result <> nil do
  begin
    if Result.index = thisIndex then
      break;

    Result := ABinaryLeaf(Result.Next);
  end;
end;

// Determine whether the tree has the specified leaf
function ABinaryTree.hasLeafWithKey(const thisKey: TSortKey): boolean;

begin
  result := false;

  if MyRoot <> nil then
    result := ABinaryLeaf(MyChild).hasKey(thisKey);
end;

// Determine whether the tree has the specified leaf
function ABinaryTree.hasLeafWithKey(const thisKey: string): boolean;

begin
  result := Self.hasLeafWithKey(Charstring.hashValueOf(thisKey));
end;

{* Inserts \p Leaf into the tree.  The insertion will only occur if the tree
   does not already have a node with a sort key that matches the one stored
   in \p Leaf.

   \return A reference to \p Leaf if the insertion was successful; #nil or a
   reference to another leaf if not.  If another leaf already existed in the
   tree with the same sort key as the one to be inserted, a reference to that
   leaf will be returned.  If \p Leaf is #nil, then the routine will return
   #nil.
*}
function ABinaryTree.insertLeaf(const Leaf: ABinaryLeaf): ABinaryLeaf;

begin
  Result := nil;
  if Leaf <> nil then
  begin
    if MyRoot <> nil then
      Result := MyRoot.insert(Leaf)

    else begin
      MyRoot := Leaf;
      Result := Leaf;
    end;

    if Result = Leaf then
    begin
      // Set the index of the node
      Leaf.myIndex := myChildCount;
      inc(myChildCount);

      if MyChild <> nil then
        MyChild.append(Leaf);

      MyChild := Leaf;
    end;
  end;
end;

// Add a new node to the tree
function ABinaryTree.addLeaf(const Leaf: ABinaryLeaf;
  const freeOnFailure: boolean = false): boolean;

begin
  { Attempt to insert the leaf and check to see whether it succeeded.
    On success, the return value of Self.insert will be the same pointer as
    Leaf itself.
  }
  result := (Leaf = Self.insertLeaf(Leaf));

  { If the insertion failed and the caller wants us to destroy the node on
    failure, then do so.
  }
  if (result = false) and (freeOnFailure) and (Leaf <> nil) then
    Leaf.free;
end;

{* Removes the node with the specified sort key from the tree.  Any existing
   subtrees of the node are reinserted into the tree at the appropriate
   positions.  The node itself is detached from its subtrees and siblings, but
   it is NOT destroyed.  The caller should destroy the node if so desired,
   or a call should be made to ABinaryLeaf::delete() instead of this method.

   \return The node that was removed from the tree, if one with \p thisKey was
   found; #nil otherwise.
*}
function ABinaryTree.removeLeaf(const thisKey: TSortKey;
  const updateIndices: boolean = true): ABinaryLeaf;

var
  // Refer to the left and right subtrees of the node to be deleted
  LeftSubtree, RightSubtree: ABinaryLeaf;
  // Refers to the insertion point of the node to be deleted
  InsertionPoint: ABinaryLeafPointer;

begin
  result := nil;
  if MyRoot = nil then
    exit;

  // Fetch the node to be deleted
  Result := Self.fetchLeaf(thisKey);
  if Result = nil then
    exit;

  // Get the left and right subtrees of the node
  LeftSubtree := Result.LeftTree;
  RightSubtree := Result.RightTree;

  // Find the insertion point of the node
  InsertionPoint := @MyRoot;

  if Result <> MyChild then
    InsertionPoint := MyRoot.insertionPointOf(thisKey);

  // Detach the node
  Result.detach;
  // Nullify the insertion point
  InsertionPoint^ := nil;

  // If the node removed is the youngest, then the second youngest becomes
  // the youngest once more
  if Result = MyChild then
    MyChild := ABinaryLeaf(MyChild.Previous);

  // Once for the node being deleted, and...
  dec(myChildCount);

  // ...if we are NOT automatically updating the indices...
  if not(updateIndices) then
  begin
    { ...once each for left and right subtrees, in order to keep the value of
      myChildCount correct.  Since the left and right subtrees will be
      reinserted, they will increase the child count once more.
    }
    if LeftSubtree <> nil then
      dec(myChildCount);

    if RightSubtree <> nil then
      dec(myChildCount);
  end;

  // Reinsert the subtrees of the node
  Self.insert(LeftSubtree);
  Self.insert(RightSubtree);

  // Update indices, if specified
  if updateIndices then
    Self.reindex;
end;

{* Removes the node with a sort key that matches the hash value of \p thisKey.
   Any existing subtrees of the node will be reinserted into the tree at the
   appropriate positions.

   The node itself is detached from its subtrees and siblings, but
   it is NOT destroyed.  The caller should destroy the node if so desired,
   or a call should be made to ABinaryLeaf::delete() instead of this method.

   \return The node that was removed from the tree, if one with \p thisKey was
   found; #nil otherwise.
*}
function ABinaryTree.removeLeaf(const thisKey: string;
  const updateIndices: boolean = true): ABinaryLeaf;

begin
  Result := Self.removeLeaf(Charstring.hashValueOf(thisKey), updateIndices);
end;

{* Deletes the node that has \p thisKey from the tree.

   Any subtrees of the node are reinserted into the tree.

   \return #true if a node with \p thisKey was found and deleted; #false
   otherwise.
*}
function ABinaryTree.deleteLeaf(const thisKey: TSortKey;
  const updateIndices: boolean = true): boolean;

var
  // Refers to the node to delete
  ThisNode: ANode;

begin
  result := false;

  ThisNode := Self.removeLeaf(thisKey, updateIndices);

  if ThisNode <> nil then
  begin
    ThisNode.free;
    result := true;
  end;
end;

{* Deletes the node that has \p thisKey from the tree.

   Any subtrees of the node are reinserted into the tree.

   \return #true if a node with \p thisKey was found and deleted; #false
   otherwise.
*}
function ABinaryTree.deleteLeaf(const thisKey: string;
  const updateIndices: boolean = true): boolean;

begin
  result := Self.deleteLeaf(Charstring.hashValueOf(thisKey), updateIndices);
end;

// Reindex the tree
procedure ABinaryTree.reindex;

var
  // Loop counter
  i: longword;
  // Refers to a node from the tree
  ThisLeaf: ABinaryLeaf;

begin
  i := 0;
  ThisLeaf := MyRoot;

  // Start at zero and reindex each subsequent node
  while ThisLeaf <> nil do
  begin
    ThisLeaf.myIndex := i;
    inc(i);

    ThisLeaf := ABinaryLeaf(ThisLeaf.next);
  end;

  myChildCount := i;
end;

// Construct a shallow copy of the other object
function ABinaryTree.shallowCopyFrom(const Other: AnObject): boolean;

var
  // Used to traverse the other tree
  ThisLeaf: ABinaryLeaf;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ABinaryTree) then
    result := false;

  if result then
  begin
    ThisLeaf := ABinaryTree(Other).Root;

    while ThisLeaf <> nil do
    begin
      Self.addLeaf(ABinaryLeaf(MyLeafType.shallowCopyOf(ThisLeaf)), true);
      ThisLeaf := ABinaryLeaf(ThisLeaf.Next);
    end;
  end;
end;

// Write the tree and its nodes to the specified stream
function ABinaryTree.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Write the number of children to the stream first
  result := Dest.write(sizeof(myChildCount), myChildCount);

  // Write each node
  if MyRoot <> nil then
    result := result + MyChild.writeTo(Dest);
end;

// Print the tree and its nodes to the specified stream
function ABinaryTree.printSelfTo(const Dest: AStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  // Print ourselves
  result := Dest.printEach([Self.toString, suffix]);

  // Print each node
  if MyRoot <> nil then
    result := result + MyRoot.printTo(Dest, prefix, suffix);
end;

// CanStream implementation ----------------------------------------------------
{* Reads the binary tree and its nodes from \p Source.  This routine first reads
   the number of nodes from the stream, then iteratively constructs and inserts
   new leaves.  ABinaryLeaf.readFrom is called on each new leaf before it is
   inserted into the tree.

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function ABinaryTree.readFrom(const Source: AStream): TStreamIOSize;

var
  // Refers to a leaf that has been re-created from the stream
  NewLeaf: ABinaryLeaf;
  // Stores the number of entries in the binary tree
  entryCount: longword;

begin
  result := 0;

  if Source <> nil then
  begin
    // Retrieve the entry count from the stream
    entryCount := 0;
    result := Source.read(sizeof(entryCount), entryCount);

    // Rebuild the tree
    while entryCount > 0 do
    begin
      NewLeaf := ABinaryLeaf(MyLeafType.new);

      if NewLeaf <> nil then
      begin
        // Read the leaf from the stream
        result := result + NewLeaf.readFrom(Source);
        // Insert the leaf into the tree
        Self.addLeaf(NewLeaf, true);
      end;

      dec(entryCount);
    end;

  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Returns a string representation of the binary tree.  This string is formatted
   as follows: the class name of the tree followed by the number of nodes it
   contains.

   \return A string representation of the tree, as described above.
*}
function ABinaryTree.toString: string;

begin
  if myChildCount <> 1 then
    result := format(bntrStringRepresentationPlural, [
      Self.displayName, myChildCount
    ])

  else
    result := format(bntrStringRepresentationSingular, [
      Self.displayName, myChildCount
    ]);
end;

{* Prints a string representation of the tree to \p Dest.  The representation
   used is that produced by ABinaryTree::toString().  The routine then calls
   ABinaryLeaf::printTo() on its root node, which ensures that each node in
   the tree is also printed to \p Dest.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ABinaryTree.printTo(const Dest: AStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    if prefix = '' then
      prefix := bntrDefaultNodePrintingPrefix;

    if suffix = '' then
      suffix := bntrDefaultPrintingSuffix;

    // Print the left subtree
    if MyLeftTree <> nil then
      result := MyLeftTree.printTo(Dest, prefix, suffix);

    // Print ourselves
    result := result + Self.printSelfTo(Dest, prefix, suffix);

    // Print the right subtree
    if MyRightTree <> nil then
      result := MyRightTree.printTo(Dest, prefix, suffix);
  end;
end;

// CanIterate implementation ---------------------------------------------------
/// Construct an iterator for the tree
function ABinaryTree.Iterator: AnIterator;

begin
  Result := ABinaryTreeIterator.over(Self);
end;

// Properties ------------------------------------------------------------------
// Retrieve the type of leaf managed by the tree
function ABinaryTree.LeafType: ANodeClass;

begin
  Result := MyLeafType;
end;

/// Retrieve the root node of the tree
function ABinaryTree.Root: ABinaryLeaf;

begin
  result := MyRoot;
end;

/// Retrieve the most recently added node in the tree
function ABinaryTree.YoungestNode: ABinaryLeaf;

begin
  result := ABinaryLeaf(MyChild);
end;

// ABinaryTreeDictionary *******************************************************
// Initializer
function ABinaryTreeDictionary.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := ABinaryTree;
  end;
end;
{$ENDIF}
