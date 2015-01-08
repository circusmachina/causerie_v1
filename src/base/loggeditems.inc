{ loggeditems.inc: Standard items that can be logged to instances of ALog,
    including hints, notes, warnings, errors, and fatal errors.

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
// ALoggedItem *****************************************************************
{ This class represents a basic item that can be logged to an instance of
  @link(ALog).  While it is possible to log anything to @link(ALog), by means
  of @link(ALog.print), classes that descend from @link(ALoggedItem) will also
  update their associated counters when logged, providing a way to track how
  many of each type of item is logged.

  @link(ALoggedItem) derives from @link(AnException) so that it can be raised
  as one, if need be.  This functionality is often used to raise instances of
  @link(ALoggedError) and @link(ALoggedFatalError) after they have been logged.
}
type ALoggedItem = class(AnException)
  protected
    { Stores the name of the log counter to increment when the item is logged }
    myCounterName: string;

  public
    { Initializer }
    function init: boolean; override;

    { Construct a shallow copy of the other object.

      This method extends the behavior inherited from
      @link(AnException.shallowCopyFrom): it calls that method, then checks
      to see if @code(Other) is an instance of @link(ALoggedItem).  If so,
      it copies the value of @link(ALoggedItem.counterName) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(AnException.toString) so that only the message part of the class
      is returned, not its display name.
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the name of the counter that will be incremented when instances
      of this class are logged to @link(ALog).  In the base implementation of
      @link(ALoggedItem), this counter is named by @link(lgitCounterName) and
      is set to the generic name @code(messages) by default.
    }
    function counterName: string; virtual;
end;

// ALoggedNote *****************************************************************
{ This class represents a simple note that is logged.  It is defined such that
  the counter it will increment upon being logged is the one specified by
  @link(lgntCounterName) -- by default, a counter named @code(notes).

  It also changes the string representation of itself produced when its
  @code(toString) method is called, to include a prefix that indicates it is a
  note.  This prefix is defined by @link(lgntStringRepresentation).
}
type ALoggedNote = class(ALoggedItem)
  public
    { Initializer }
    function init: boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(Classwork.ALoggedItem.toString) so that a prefix is included with the
      message that indicates it is a note.  This prefix is defined by
      @link(lgntStringRepresentation).
    }
    function toString: AnsiString; override;
end;

// ALoggedHint *****************************************************************
{ This class represents a hint that is logged.  It is defined such that
  the counter it will increment upon being logged is the one specified by
  @link(lghtCounterName) -- by default, a counter named @code(hints).

  It also changes the string representation of itself produced when its
  @code(toString) method is called, to include a prefix that indicates it is a
  hint.  This prefix is defined by @link(lghtStringRepresentation).
}
type ALoggedHint = class(ALoggedItem)
  public
    { Initializer }
    function init: boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(ALoggedItem.toString) so that a prefix is included with the
      message that indicates it is a hint.  This prefix is defined by
      @link(lghtStringRepresentation).
    }
    function toString: AnsiString; override;
end;

// ALoggedWarning **************************************************************
{ This class represents a warning that is logged.  It is defined such that
  the counter it will increment upon being logged is the one specified by
  @link(lgwrCounterName) -- by default, a counter named @code(warnings).

  It also changes the string representation of itself produced when its
  @code(toString) method is called, to include a prefix that indicates it is a
  warning.  This prefix is defined by @link(lgwrStringRepresentation).
}
type ALoggedWarning = class(ALoggedItem)
  public
    { Initializer }
    function init: boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(ALoggedItem.toString) so that a prefix is included with the
      message that indicates it is a warning.  This prefix is defined by
      @link(lgwrStringRepresentation).
    }
    function toString: AnsiString; override;
end;

// ALoggedError ****************************************************************
{ This class represents a error that is logged.  It is defined such that
  the counter it will increment upon being logged is the one specified by
  @link(lgerCounterName) -- by default, a counter named @code(errors).

  It also changes the string representation of itself produced when its
  @code(toString) method is called, to include a prefix that indicates it is an
  error.  This prefix is defined by @link(lgerStringRepresentation).
}
type ALoggedError = class(ALoggedItem)
  public
    { Initializer }
    function init: boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(ALoggedItem.toString) so that a prefix is included with the
      message that indicates it is an error.  This prefix is defined by
      @link(lgerStringRepresentation).
    }
    function toString: AnsiString; override;
end;

// ALoggedFatalError ***********************************************************
{ This class represents a hint that is logged.  It is defined such that
  the counter it will increment upon being logged is the one specified by
  @link(lgfeCounterName) -- by default, a counter named @code(fatal errors).

  It also changes the string representation of itself produced when its
  @code(toString) method is called, to include a prefix that indicates it is a
  hint.  This prefix is defined by @link(lgfeStringRepresentation).
}
type ALoggedFatalError = class(ALoggedItem)
  public
    { Initializer }
    function init: boolean; override;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the item, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(ALoggedItem.toString) so that a prefix is included with the
      message that indicates it is a fatal error.  This prefix is defined by
      @link(lgfeStringRepresentation).
    }
    function toString: AnsiString; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls the way an instance of @link(ALoggedNote) is
    represented as a string by @link(ALoggedNote.toString).

    The string placeholder is filled with the message associated with the note,
    as returned by a call to @link(AnException.message) when applied to the
    instance of @link(ALoggedNote).
  }
  lgntStringRepresentation = 'note: %s';

  { This string controls the way an instance of @link(ALoggedHint) is
    represented as a string by @link(ALoggedHint.toString).

    The string placeholder is filled with the message associated with the note,
    as returned by a call to @link(AnException.message) when applied to the
    instance of @link(ALoggedHint).
  }
  lghtStringRepresentation = 'hint: %s';

  { This string controls the way an instance of @link(ALoggedWarning) is
    represented as a string by @link(ALoggedWarning.toString).

    The string placeholder is filled with the message associated with the note,
    as returned by a call to @link(AnException.message) when applied to the
    instance of @link(ALoggedWarning).
  }
  lgwrStringRepresentation = 'warning: %s';

  { This string controls the way an instance of @link(ALoggedError) is
    represented as a string by @link(ALoggedError.toString).

    The string placeholder is filled with the message associated with the note,
    as returned by a call to @link(AnException.message) when applied to the
    instance of @link(ALoggedError).
  }
  lgerStringRepresentation = 'error: %s';

  { This string controls the way an instance of @link(ALoggedFatalError) is
    represented as a string by @link(ALoggedFatalError.toString).

    The string placeholder is filled with the message associated with the note,
    as returned by a call to @link(AnException.message) when applied to the
    instance of @link(ALoggedFatalError).
  }
  lgfeStringRepresentation = 'fatal: %s';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedItem) are logged.
  }
  lgitCounterName = 'messages';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedNote) are logged.
  }
  lgntCounterName = 'notes';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedHint) are logged.
  }
  lghtCounterName = 'hints';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedWarning) are logged.
  }
  lgwrCounterName = 'warnings';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedError) are logged.
  }
  lgerCounterName = 'errors';

  { This string is used as the default name for the counter to be incremented
    when instances of @link(ALoggedFatalError) are logged.
  }
  lgfeCounterName = 'fatal errors';
{$ENDIF}
{$IFDEF ReadImplementation}
// ALoggedItem *****************************************************************
{* Initializes a new instance of ALoggedItem.  This routine sets
   ALoggedMessage.counterName to the value of @link(lgitCounterName) before
   calling the inherited routine.

   \return #true if initialization was successful; #false if not.
*}
function ALoggedItem.init: boolean;

begin
  myCounterName := lgitCounterName;

  result := inherited init;
end;

// Construct a shallow copy of the other object
function ALoggedItem.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see if we're compatible with the other object
  if not(Other is ALoggedItem) then
    result := false;

  if result then
  begin
    myCounterName := ALoggedItem(Other).counterName;
  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the message.  This representation is
   the value of ALoggedItem.message.

   \return A string representation of the message, as described above.
*}
function ALoggedItem.toString: string;

begin
  result := myMessage;
end;

// Properties ------------------------------------------------------------------
/// Retrieve the counter named assigned to the item
function ALoggedItem.counterName: string;

begin
  result := myCounterName;
end;

// ALoggedNote *****************************************************************
/// Initializer
function ALoggedNote.init: boolean;

begin
  myCounterName := lgntCounterName;

  result := inherited init;
end;

// IsPrintable implementation --------------------------------------------------
/// Constructs a string representation of the note
function ALoggedNote.toString: string;

begin
  result := format(lgntStringRepresentation, [myMessage]);
end;

// ALoggedHint *****************************************************************
/// Initializer
function ALoggedHint.init: boolean;

begin
  myCounterName := lghtCounterName;

  result := inherited init;
end;

// IsPrintable implementation --------------------------------------------------
/// Constructs a string representation of the hint
function ALoggedHint.toString: string;

begin
  result := format(lghtStringRepresentation, [myMessage]);
end;

// ALoggedWarning **************************************************************
/// Initializer
function ALoggedWarning.init: boolean;

begin
  myCounterName := lgwrCounterName;

  result := inherited init;
end;

// IsPrintable implementation --------------------------------------------------
/// Constructs a string representation of the warning
function ALoggedWarning.toString: string;

begin
  result := format(lgwrStringRepresentation, [myMessage]);
end;

// ALoggedError ****************************************************************
/// Initializer
function ALoggedError.init: boolean;

begin
  myCounterName := lgerCounterName;

  result := inherited init;
end;

// IsPrintable implementation --------------------------------------------------
/// Constructs a string representation of the hint
function ALoggedError.toString: string;

begin
  result := format(lgerStringRepresentation, [myMessage]);
end;

// ALoggedFatalError ***********************************************************
/// Initializer
function ALoggedFatalError.init: boolean;

begin
  myCounterName := lgfeCounterName;

  result := inherited init;
end;

// IsPrintable implementation --------------------------------------------------
/// Constructs a string representation of the hint
function ALoggedFatalError.toString: string;

begin
  result := format(lgfeStringRepresentation, [myMessage]);
end;
{$ENDIF}

