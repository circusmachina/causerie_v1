{ loggingobjects.inc: A class that can log.

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
// ALoggingObject **************************************************************
{ This class represents a generic object that is capable of logging items to
  a given log.  The base class does not create a log itself; it relies on
  the implementations of its descendants to do that.  It does provide
  several utility methods that can be called by descendant implementations in
  order to log notes, hints, warnings, errors, and fatal errors.
}
type ALoggingObject = class(APrintingObject)
  protected
    { Refers to the log used by the instance to record status information and
      errors.
    }
    MyLog: ALog;

  public
    { Initializer }
    function init: boolean; override;

    { Log the specified items to the log.  This method is simply a pass-through
      method for @link(ALog.print).
    }
    function log(const items: array of const): TStreamIOSize; virtual;

    { Log a note to the log.

      This method constructs an instance of @link(ALoggedNote), which is
      then logged.  @code(message) and @code(params) are passed to the
      constructor for @link(ALoggedNote).

      If @code(retain) is @true, then the instance of @link(ALoggedNote)
      constructed by this routine is returned to the caller; this allows the
      caller to both log the note and raise it as an exception with one
      statement: @longCode(#
        // Log a note and raise an error
        raise Self.logNoteBecause(noteExecutionReachedHere, []);
      #)

      Otherwise, the note is freed and the call returns @nil.
    }
    function logNoteBecause(const message: string;
      const params: array of const;
      const retain: boolean = false): ALoggedNote; virtual;

    { Log a hint to the log.

      This method constructs an instance of @link(ALoggedHint), which is then
      logged.  @code(message) and @code(params) are passed to the constructor
      for @link(ALoggedHint).

      If @code(retain) is @true, then the instance of @link(ALoggedHint)
      constructed by this routine is returned to the caller; this allows the
      caller to both log the hint and raise it as an exception with one
      statement: @longCode(#
        // Log a hint and raise an error
        raise Self.logHintBecause(hintExecutionReachedHere, []);
      #)

      Otherwise, the hint is freed and the call returns @nil.
    }
    function logHintBecause(const message: string;
      const params: array of const;
      const retain: boolean = false): ALoggedHint; virtual;

    { Log a warning to the log.

      This method constructs an instance of @link(ALoggedWarning), which is then
      logged.  @code(message) and @code(params) are passed to the constructor
      for @link(ALoggedWarning).

      If @code(retain) is @true, then the instance of @link(ALoggedWarning)
      constructed by this routine is returned to the caller; this allows the
      caller to both log the warning and raise it as an exception with one
      statement: @longCode(#
        // Log a warning and raise an error
        raise Self.logWarningBecause(warnExecutionReachedHere, []);
      #)

      Otherwise, the warning is freed and the call returns @nil.
    }
    function logWarningBecause(const message: string;
      const params: array of const;
      const retain: boolean = false): ALoggedWarning; virtual;

    { Log an error to the log.

      This method constructs an instance of @link(ALoggedError), which is then
      logged.  @code(message) and @code(params) are passed to the constructor
      for @link(ALoggedError).

      If @code(retain) is @true, then the instance of @link(ALoggedError)
      constructed by this routine is returned to the caller; this allows the
      caller to both log the error and raise it as an exception with one
      statement: @longCode(#
        // Log and raise an error
        raise Self.logErrorBecause(errExecutionReachedHere, []);
      #)

      Otherwise, the error is freed and the call returns @nil.
    }
    function logErrorBecause(const message: string;
      const params: array of const;
      const retain: boolean = false): ALoggedError; virtual;

    { Log a fatal error to the log.

      This method constructs an instance of @link(ALoggedFatalError), which
      is then logged.  @code(message) and @code(params) are passed to the
      constructor for @link(ALoggedFatalError).

      If @code(retain) is @true, then the instance of @link(ALoggedFatalError)
      constructed by this routine is returned to the caller; this allows the
      caller to both log the error and raise it as an exception with one
      statement: @longCode(#
        // Log a hint and raise an error
        raise Self.logFatalErrorBecause(ferrExecutionReachedHere, []);
      #)

      Otherwise, the error is freed and the call returns @nil.
    }
    function logFatalErrorBecause(const message: string;
      const params: array of const;
      const retain: boolean = false): ALoggedFatalError; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the log used by the instance to log status
      messages and warnings.  The caller should NOT attempt to free this
      reference.
    }
    function Log: ALog; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ALoggingObject **************************************************************
// Initializer
function ALoggingObject.init: boolean;

begin
  MyLog := nil;

  result := inherited init;
end;

// Log the specified items to the log
function ALoggingObject.log(const items: array of const): TStreamIOSize;

begin
  result := 0;

  if MyLog <> nil then
    result := MyLog.print(items);
end;

// Log a note to the log
function ALoggingObject.logNoteBecause(const message: string;
  const params: array of const; const retain: boolean = false): ALoggedNote;

begin
  Result := ALoggedNote.because(message, params);

  Self.log([Result]);

  if retain = false then
    SysUtils.freeAndNil(Result);
end;

// Log a hint to the log
function ALoggingObject.logHintBecause(const message: string;
  const params: array of const; const retain: boolean = false): ALoggedHint;

begin
  Result := ALoggedHint.because(message, params);

  Self.log([Result]);

  if retain = false then
    SysUtils.freeAndNil(Result);
end;

// Log a warning to the log
function ALoggingObject.logWarningBecause(const message: string;
  const params: array of const; const retain: boolean = false): ALoggedWarning;

begin
  Result := ALoggedWarning.because(message, params);

  Self.log([Result]);

  if retain = false then
    SysUtils.freeAndNil(Result);
end;

// Log an error to the log
function ALoggingObject.logErrorBecause(const message: string;
  const params: array of const; const retain: boolean = false): ALoggedError;

begin
  Result := ALoggedError.because(message, params);

  Self.log([Result]);

  if retain = false then
    SysUtils.freeAndNil(Result);
end;

// Log a fatal error to the log
function ALoggingObject.logFatalErrorBecause(const message: string;
  const params: array of const;
  const retain: boolean = false): ALoggedFatalError;

begin
  Result := ALoggedFatalError.because(message, params);

  Self.log([Result]);

  if retain = false then
    SysUtils.freeAndNil(Result);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the log used
function ALoggingObject.Log: ALog;

begin
  Result := MyLog;
end;
{$ENDIF}

