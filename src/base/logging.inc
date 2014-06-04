{ loggeditems.inc: Various types of logs for debug and error output.

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
// ALog ************************************************************************
{ This class represents a generic log, which can be used to output status and
  error messages to an assigned stream.  Any type of information can be logged
  to @link(ALog) by means of @link(ALog.write); however, when the item to be
  logged is an instance of @link(ALoggedItem) or its descendants, then the
  logging of that instance will be recorded in the list of counters maintained
  by the log.
}
type ALog = class(APrintingObject)
  protected
    { Stores counters for errors, hints, warnings, etc. }
    MyCounters: ACounterList;
    { Refers to the stream used for output from the log }
    MyOutput: AStream;
    { Indicates whether the log owns its output stream, and whether it will
      free this stream when the log itself is destroyed.
    }
    myOwnOutput: boolean;

  public
    { Construct a new instance of @link(ALog) that will output status messages
      and errors to the given stream.

      If @link(ownStream) is @true, then the log will assume ownership of
      @link(Dest) and will free it when it is, itself, freed.  This allows
      instances of a log to be constructed and assigned to a stream like so:
      @longcode(#
        // Log to 'stdout'
        MyLog := ALog.toStream(AStandardOutputStream.new, true);
      #)
    }
    constructor toStream(const Dest: AStream;
      const ownStream: boolean = false); virtual;

    { Initializer }
    function init: boolean; override;

    { Frees the log and all resources associated with it.

      If @link(ALog.ownsOutput) is @true, then this routine will ensure that
      @link(ALog.Output) is freed as well.  This is useful behavior when it
      is desirable to construct a log and assign it to a stream of its own in
      a single statement, as follows:
      @longcode(#
        // Log to 'stdout'
        MyLog := ALog.toStream(AStandardOutputStream.new, true);
      #)
    }
    destructor destroy; override;

    { Write one or more items to the log.

      This function accepts all types of values.  Integer and floating-point
      values are converted to strings before being written to the log's output
      stream.  Object instances are checked to see if they implement
      @link(CanPrint); if so, their @code(printTo) method is called and directed
      to output to the log's output stream.

      If an instance of @link(ALoggedItem) or its descendants is passed to this
      method, then the counter associated with that instance (as determined by
      a call to @link(ALoggedItem.counterName)) is incremented.  If the
      counter does not yet exist in @link(ALog.Counters), then it is created.

      Internally, after checking to see if there are items in @code(items)
      that are instances of @link(ALoggedItem) and incrementing the counters
      associated with any found, this method calls @link(AStream.printEach) on
      its output stream.

      @return(The total number of bytes written to the log's output stream.)
    }
    function write(const items: array of const): TStreamIOSize; virtual;

  // CanPrint implementation ---------------------------------------------------
    { Print a string representation of the log to the specified stream.

      This method does not actually print a string representation of the log
      itself; rather, it calls @link(ACounterList.printTo) on its list of
      counters.  @code(prefix) and @code(suffix) are passed to that routine
      and are not used by this one.  This behavior ensures that a summary of
      all logged items is printed every time this method is called.

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printTo(const Dest: AStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

  // Properties ----------------------------------------------------------------
    { Retrieve a reference to the counter with the specified name.  The caller
      should NOT free this reference; the log will free it when it is, itself,
      freed.

      If the counter does not exist in the log's list of counted items, then
      this routine will return @nil.
    }
    function Counter(const thisName: string): ACounter; virtual;

    { Retrieve a reference to the list of all counted items maintained by the
      log.  This reference should NOT be freed by the caller; it will be freed
      by the log when the log is, itself, freed.
    }
    function Counters: ACounterList; virtual;

    { Retrieve a reference to the output stream used by the log to print status
      messages and errors.

      The reference returned by this routine should NOT be freed by the caller.
      If @link(ALog.ownsOutput) is @true, then the stream will be freed when the
      log is, itself, freed.
    }
    function Output: AStream; virtual;

    { Retrieve whether or not the log assumes ownership of its output stream.

      If this method returns @true, it indicates that the log will free its
      output stream when it is, itself, freed.  Otherwise, it is the
      responsibility of the routine that constructed the log to ensure that the
      output stream is freed when it is no longer required.
    }
    function ownsOutput: boolean; virtual;

    { Set whether or not the log assumes ownership of its output stream.

      If @code(flag) is @true, then the log will assume ownership of its
      output stream and will attempt to free that stream when the log is,
      itself, freed.  If set to @false, then the log will not attempt to free
      its output stream and it will be the responsibility of the caller to
      ensure that stream is freed when it is no longer required.
    }
    function setOwnsOutput(const flag: boolean): boolean; virtual;
end;

// AFileLog ********************************************************************
{ This class represents a log that will log to a file stream.  The class
  expands on what it inherits from @link(ALog) by allowing instances of itself
  to be constructed with the name of a file.  The constructor will create the
  output file stream and assume ownership of it.
}
type AFileLog = class(ALog)
  public
    { Construct a new instance of @link(AFileLog) that will output to the
      named file.  If @code(overwrite) is @true and the named file already
      exists, then its contents will be destroyed before the log begins to
      output.  Otherwise, the log will append to the file if it exists, and
      attempt to create it if it does not.

      This method constructs and assumes ownership of an instance of
      @link(AFileOutputStream).
    }
    constructor toFileNamed(const thisName: string;
      const overwrite: boolean = false); virtual;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the log, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from @link(ALog), which
      simply prints the display name of the class.  Instead, it returns the
      name of the output file to which items are logged.
    }
    function toString: AnsiString; override;
end;

// AConsoleLog *****************************************************************
{ This class represents a log that will log to either @code(stdout) or
  @code(stderr).  The two constructors provided by the class will construct
  the appropriate instance of @link(AStandardOutputStream) or
  @link(AStandardErrorStream) to use for output.
}
type AConsoleLog = class(ALog)
  public
    { Construct a new instance of @link(AConsoleLog) that will output to
      @code(stdout).  This method constructs and assumes ownership of an
      instance of @link(AStandardOutputStream) which will be used for output.
    }
    constructor toStdOut; virtual;

    { Construct a new instance of @link(AConsoleLog) that will output to
      @code(stderr).  This method constructs and assumes ownership of an
      instance of @link(AStandardErrorStream) which will be used for
      output.
    }
    constructor toStdErr; virtual;
end;

// ABufferedLog ****************************************************************
{ This class represents a log that will log to a string stream.  The class
  expands upon what it inherits from @link(ALog) to automatically construct and
  assume ownership of an instance of @link(AStringStream), which it uses for
  output.
}
type ABufferedLog = class(ALog)
  public
    { Construct a new instance of @link(ABufferedLog) that will output to
      a string stream.  This method constructs and assumes ownership of an
      instance of @link(AStringStream) which will be used for output.
    }
    constructor new; override;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ALog ************************************************************************
{* Constructs a new instance of ALog that will output to \p ThisStream.
   The message log does not assume ownership of \p ThisStream and will not free
   it when the log itself is freed unless AMessageLog.ownsOutput is set to
   #true.

   \return A new instance of AMessageLog on success; #nil on failure.
*}
constructor ALog.toStream(const Dest: AStream;
  const ownStream: boolean = false);

begin
  inherited new;

  MyOutput := Dest;
  myOwnOutput := ownStream;
end;

{* Initializes a new instance of ALog.  An instance of ABinaryTree is
   constructed to manage AMessageLog.Counters.  A default counter named
   "message" is added.  If the binary tree instance could not be constructed,
   then this routine returns #false (and will trigger an exception from the
   default implementation of AnObject.new).

   \return #true if the message log was initialized successfully; #false if
   not.
*}
function ALog.init: boolean;

begin
  result := inherited init;

  if result = true then
  begin
    MyCounters := ACounterList.new;

    MyOutput := nil;
    myOwnOutput := false;
    result := MyCounters <> nil;
  end;
end;

{* Destroys the instance of AMessageLog, freeing its counters.  If
   AMessageLog.ownsOutput is #true, the output stream will also be freed.
*}
destructor ALog.destroy;

begin
  if MyCounters <> nil then
    MyCounters.free;

  if (MyOutput <> nil) and (myOwnOutput) then
    MyOutput.free;

  inherited destroy;
end;

/// Write the specified items to the log
function ALog.write(const items: array of const): TStreamIOSize;

var
  // A loop counter
  i: longword;

begin
  result := 0;

  if System.length(items) = 0 then
    exit;

  for i := 0 to (System.length(items) - 1) do
  begin
    // Check each item to see if we have an instance of ALoggedItem
    if items[i].vType = vtObject then
    begin
      if items[i].vObject is ALoggedItem then
        // If the item is an instance of ALoggedItem, then increment the
        // associated counter
        with items[i].vObject as ALoggedItem do
        begin
          if MyCounters.hasKey(counterName) then
            Self.Counter(counterName).increment

          else
            MyCounters.insert(ACounter.named(counterName, 1));
        end;
    end;
  end;

  // Write the objects
  result := Self.Output.printEach(items);
end;

// CanPrint implementation -----------------------------------------------------
{* Prints a string representation of the log to \p Dest.  This routine calls
   ALog.toString to provide the opening line of the summary that it
   generates by calling ACounter.printTo for each counter managed by the
   message log.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ALog.printTo(const Dest: AStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := MyCounters.printTo(Dest, prefix, suffix);
end;

// Properties ------------------------------------------------------------------
{* Retrieves the named counter from the list of counters being managed by the
   log.

   \return An ACounter instance that represents the named counter on success;
   #nil on failure.  The call will failure of \p thisName is invalid (an empty
   string) or if the named counter was not found in the list of counters being
   managed by the message log.
*}
function ALog.Counter(const thisName: string): ACounter;

begin
  Result := nil;

  if thisName <> '' then
  begin
    Result := ACounter(MyCounters.fetch(thisName));
  end;
end;

/// Retrieves the list of counters associated with the log
function ALog.Counters: ACounterList;

begin
  Result := MyCounters;
end;

/// Retrieves the output stream used by the log
function ALog.Output: AStream;

begin
  Result := MyOutput;
end;

/// Retrieves whether or not the log owns its output stream
function ALog.ownsOutput: boolean;

begin
  result := myOwnOutput;
end;

/// Sets whether or not the log owns its output stream
function ALog.setOwnsOutput(const flag: boolean): boolean;

begin
  result := myOwnOutput;
  myOwnOutput := flag;
end;

// AFileLog ********************************************************************
/// Constructor
constructor AFileLog.toFileNamed(const thisName: string;
  const overwrite: boolean = false);

begin
  inherited new;

  if thisName <> '' then
  begin
    MyOutput := AFileOutputStream.toFileNamed(thisName, overwrite);
    myOwnOutput := true;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Return a string representation of the log (log file name)
function AFileLog.toString: AnsiString;

begin
  result := MyOutput.toString;
end;

// AConsoleLog *****************************************************************
/// Constructor
constructor AConsoleLog.toStdOut;

begin
  inherited new;

  MyOutput := AStandardOutputStream.new;
  myOwnOutput := true;
end;

constructor AConsoleLog.toStdErr;

begin
  inherited new;

  MyOutput := AStandardErrorStream.new;
  myOwnOutput := true;
end;

// ABufferedLog ****************************************************************
/// Constructor
constructor ABufferedLog.new;

begin
  inherited new;

  MyOutput := AStringStream.new('');
  myOwnOutput := true;
end;
{$ENDIF}

