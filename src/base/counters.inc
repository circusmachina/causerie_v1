{ counters.inc: A simple named counter class and a class to manage them.

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
// ACounter ********************************************************************
{ This class represents a single named counter.  It is used primarily by
  instances of @link(ALog) to keep track of the quantities of various types of
  messages and errors that are logged.  Each named counter can have a maximum
  value and can optionally throw an exception when that value is exceeded.

  Counters descend from @link(AStringLeaf) so that they can be managed by name
  in a list of counters; this is done by @link(ACounterList).
}
type ACounter = class(AStringLeaf)
  protected
    { Stores the value of the counter }
    myValue: longword;
    { Stores the maximum value of the counter }
    myLimit: longword;
    { Indicates whether or not the counter should throw an exception when its
      maximum value is exceeded.
    }
    myNotifyOnExcess: boolean;

  public
    { Construct a new counter instance with a specific name, starting value and
      optional maximum value.

      If @code(maximumValue) is zero (0), then the counter has no maximum.
    }
    constructor named(const thisName: string; const initialValue: longword = 0;
      const maximumValue: longword = 0); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Increments the counter by the specified value.

      After incrementing the value, this method checks to see whether the
      maximum value of the counter has been exceeded; if so, and if
      @link(ACounter.notifyOnExcess) is @true, then it raises an exception.

      @raises(ACounterOverageError if all three of the following are true:
      the maximum value of the counter is specified (i.e., not zero [0]), that
      value is exceeded, and @link(ACounter.notifyOnExcess) is @true.)
    }
    procedure increment(const quantity: longword = 1); virtual;

    { Decrements the counter by the specified value.

      Counters cannot be decremented below zero (0).
    }
    procedure decrement(const quantity: longword = 1); virtual;

    { Resets the value of the counter to zero (0). }
    procedure zero; virtual;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AStringLeaf.shallowCopyFrom): it calls that routine, then checks to
      see whether @code(Other) is an instance of @link(ACounter).  If so, it
      copies the values of
      @unorderedList(
        @item(@link(ACounter.value),)
        @item(@link(ACounter.limit), and)
        @item(@link(ACounter.notifyOnExcess))
      )
      from @code(Other) to @code(Self), overwriting the values in @code(Self).

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

    { Calculate the number of bytes required to stream the counter, and just
      the counter.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.selfStreamingLength): it calls the inherited routine,
      and then adds the number of bytes required to store the current value of
      the counter, its limit, and whether or not it should raise an exception
      on overages.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the counter, and just the counter, to the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.writeSelfTo): it calls the inherited routine first, then
      writes the current value of the counter, its limit, and whether or not it
      should raise an exception on overages.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

  // CanStream implementation --------------------------------------------------
    { Read the counter from the specified stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom): it calls the inherited routine first, then
      reads the value of the counter, its limit, and whether or not it should
      raise an exception on overages from @code(Source).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

  // CanPrint implementation ---------------------------------------------------
    { Construct and return a string representation of the counter and its value,
      suitable for printing to a text-based device, such as a console.

      The string returned by this routine will include the name of the counter
      and its current value.  The format of the string is controlled by
      @link(cntrStringRepresentation).
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Return the current value of the counter. }
    function value: longword; virtual;

    { Return the maximum value of the counter.

      If @link(ACounter.notifyOnExcess) is @true, then
      @link(ACounter.increment) will raise an exception if this value is
      exceeded.
    }
    function limit: longword; virtual;

    { Set the maximum value of the counter.

      If @link(ACounter.notifyOnExcess) is @true, then
      @link(ACounter.increment) will raise an exception if the value specified
      by @code(newLimit) is exceeded.

      @return(The previous maximum value of the counter.)
    }
    function setLimit(const newLimit: longword): longword; virtual;

    { Determine whether or not @link(ACounter.increment) will raise an exception
      when the value specified by @link(ACounter.limit) is exceeded.

      By default, the initializer of @link(ACounter) sets this value to
      @true.
    }
    function notifyOnExcess: boolean; virtual;

    { Set whether or not @link(ACounter.increment) will raise an exception
      when the value specified by @link(ACounter.limit) is exceeded.

      @return(The previous value of @link(ACounter.limit).)
    }
    function setNotifyOnExcess(const flag: boolean): boolean; virtual;
end;

{ An instance of this exception class is raised by @link(ACounter.increment) if
  all of the following are true: @link(ACounter.limit) has a value that is not
  zero, that value is exceeded by a call to @link(ACounter.increment), and
  @link(ACounter.notifyOnExcess) is @true.
}
type ACounterOverageError = class(AnException);

{ This class is used by @link(ALog) and its descendants to manage a list of
  named counters: one for each type of message that can be logged.

  As with other descendants of @link(ABinaryTree), the list of counters does
  not support duplicate counter names.
}
type ACounterList = class(AStringTree)
  public
    { Initializer}
    function init: boolean; override;

    { Print a string representation of the counter list and its counters to the
      specified stream.

      This method will print a summary of each counter in the list, along with
      that counter's current value.  The header of the summary will be the value
      of @link(ctlsPrintedSummaryHeader).  Both @code(prefix) and @code(suffix)
      are used by this method, if supplied; however, the method passes the
      value of @link(ctlsDefaultPrintingPrefix) and
      @link(ctlsDefaultPrintingSuffix) to @link(ANode.printTo ACounter.printTo)
      for each named counter; this ensures that each counter is printed on its
      own line and is indented beneath the banner header.

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printSelfTo(const Dest: ATextOutputStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string is used to format the error message passed to instances of
    @link(ACounterOverageError) when @link(ACounter.increment) raises it.

    The string placeholder is filled with the name of the counter, as returned
    by a call to @link(AStringLeaf.name) applied to the @link(ACounter)
    instance.  The integer placeholder is filled with the maximum value of the
    counter, as returned by a call to @link(ACounter.limit).
  }
  cntrErrorLimitReached = 'the number of %s exceeds the limit of %d';

  { This string is used by @link(ACounter.toString) to control how a string
    representation of @link(ACounter) is produced.

    The string placeholder is filled by the name of the counter, as
    returned by call to @link(AStringLeaf.name) when applied to
    an instance of @link(ACounter).  The integer placeholder is filled with
    the current value of the counter, as returned by a call to
    @link(ACounter.value).
  }
  cntrStringRepresentation = '%s: %d';

  { This string is used by @link(ACounterList.printSelfTo) as the banner header
    printed to the destination stream before the values of each counter in
    the list are printed.
  }
  ctlsPrintedSummaryHeader = 'Total number of' + lineEnding;

  { This string is used by @link(ACounterList.printSelfTo) as the @code(prefix)
    passed to @code(ACounter.printTo).  It ensures that a tab character
    precedes the name and value of the counter, so that the name and value
    are indentend beneath the banner header that is specified by
    @link(ctlsPrintedSummaryHeader).
  }
  ctlsDefaultPrintingPrefix = #9;

  { This string is used by @link(ACounterList.printSelfTo) as the @code(suffix)
    passed to @code(ACounter.printTo).  It ensures that a newline follows
    the name of value of the counter, so that each counter occupies a single
    line of output.
  }
  ctlsDefaultPrintingSuffix = lineEnding;
{$ENDIF}
{$IFDEF ReadImplementation}
// ACounter ********************************************************************
// Constructor
constructor ACounter.named(const thisName: string;
  const initialValue: longword = 0;
  const maximumValue: longword = 0);

begin
  inherited named(thisName);

  myValue := initialValue;
  myLimit := maximumValue;
end;

{* Initializes a new instance of ACounter.  This routine sets ACounter.count
   and ACounter.limit to zero (0) and calls the inherited routine.

   \return #true if the initialization was successful; #false if not.
*}
function ACounter.init: boolean;

begin
  myValue := 0;
  myLimit := 0;
  myNotifyOnExcess := true;

  result := inherited init;
end;

{* Increments the counter by one (1).  If this causes ACounter.limit to be
   reached, an exception is thrown.
*}
procedure ACounter.increment(const quantity: longword = 1);

begin
  inc(myValue, quantity);

  if (myLimit > 0) and (myValue > myLimit) and (myNotifyOnExcess) then
    raise ACounterOverageError.because(cntrErrorLimitReached, [
      Self.name, myLimit
    ]);
end;

{* Decrements the counter by one (1), so long as ACounter.count is greater than
   zero (0).
*}
procedure ACounter.decrement(const quantity: longword = 1);

begin
  if myValue > 0 then
    dec(myValue, quantity);
end;

{* Resets ACounter.count to zero (0).
*}
procedure ACounter.zero;

begin
  myValue := 0;
end;

// Construct a shallow copy of the other object
function ACounter.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ACounter) then
    result := false;

  if result then
  begin
    myValue := ACounter(Other).value;
    myLimit := ACounter(Other).limit;
    myNotifyOnExcess := ACounter(Other).notifyOnExcess;
  end;
end;

// Calculate the number of bytes required to stream the counter
function ACounter.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine first
  result := inherited selfStreamingLength;

  inc(result, sizeof(myValue) + sizeof(myLimit) + sizeof(myNotifyOnExcess));
end;

// Write the counter to the given stream
function ACounter.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    result := inherited writeSelfTo(Dest);

    // Write the current value of the counter
    result := result + Dest.write(sizeof(myValue), myValue);
    // Write the limit of the counter
    result := result + Dest.write(sizeof(myLimit), myLimit);
    // Write the flag that indicates whether to raise an exception on overages
    result := result + Dest.write(sizeof(myNotifyOnExcess), myNotifyOnExcess);
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the counter from the given stream
function ACounter.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    result := inherited readFrom(Source);

    // Read the value of the counter
    result := result + Source.read(sizeof(myValue), myValue);
    // Read the limit of the counter
    result := result + Source.read(sizeof(myLimit), myLimit);
    // Read the flag that indicates whether to raise an exception on overages
    result := result + Source.read(sizeof(myNotifyOnExcess), myNotifyOnExcess);
  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Returns a string representation of the counter.  This string is formatted
   such that the counter's name precedes its count.  A counter named
   "syntax errors" will cause this routine to construct a representation that
   reads: "syntax errors: [count]".

   \return A string representation of the counter, which includes the value of
   ACounter.count.
*}
function ACounter.toString: string;

begin
  result := format(cntrStringRepresentation, [Self.name, myValue]);
end;

// Properties ------------------------------------------------------------------
/// Retrieves the current value of the counter
function ACounter.value: longword;

begin
  result := myValue;
end;

/// Retrieves the limit of the counter
function ACounter.limit: longword;

begin
  result := myLimit;
end;

/// Sets the limit of the counter
function ACounter.setLimit(const newLimit: longword): longword;

begin
  result := myLimit;
  myLimit := newLimit;
end;

/// Retrieve whether or not the counter will raise an exception when the limit is exceeded
function ACounter.notifyOnExcess: boolean;

begin
  result := myNotifyOnExcess;
end;

/// Set whether or not the counter will raise an exception when the limit is exceed
function ACounter.setNotifyOnExcess(const flag: boolean): boolean;

begin
  result := myNotifyOnExcess;
  myNotifyOnExcess := flag;
end;

// ACounterList ****************************************************************
// Initializer
function ACounterList.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLeafType := ACounter;
  end;
end;

// Print the list of counters to the given stream
function ACounterList.printSelfTo(const Dest: ATextOutputStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Print the prefix and banner header
    result := Dest.printEach([prefix, ctlsPrintedSummaryHeader]);

    // Print each counter
    if MyRoot <> nil then
      result := result + MyRoot.printTo(Dest, ctlsDefaultPrintingPrefix,
        ctlsDefaultPrintingSuffix);

    // Print the suffix
    result := result + Dest.print(suffix);
  end;
end;
{$ENDIF}


