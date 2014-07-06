{ applications.inc: A basic framework for representing a single process in an
    object-oriented fashion.

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
{ This class represents a single process (an application) in an object-oriented
  fashion.  It is somewhat similar to the old Borland Turbo Pascal
  @code(TApplication) class and somewhat more simplistic than the
  @code(TCustomApplication) class provided in Free Component Library for Free
  Pascal.

  The base implementation of this class supports printing a string
  representation of the class to an arbitrary stream; it also provides
  object-oriented access to command-line parameters and environment variables
  made available to the process.  This functionality is designed to provide
  support for descendant classes, as you will likely not directly instantiate
  @link(AnApplication), but will instead derive a custom class from it or one of
  its descendants (such as @link(AConsoleApplication).
}
type AnApplication = class(APrintingObject)
  protected
    { Stores the environment variables available to the process }
    MyEnvironment: ADictionary;
    { Stores the command-line parameters available to the process }
    MyParameters: AStringList;

    { Parse any and all environment variables made available to the process.
      This method separates the name of the variable from its value and stores
      both in @link(AnApplication.Environment).  It is called automatically by
      @link(AnApplication.init).
    }
    procedure parseEnvironment; virtual;

    { Parse the command line arguments made available to the process.  This
      method collects all parameters except the zeroth one (the name of the
      application being executed) into @link(AnApplication.Parameters).  It is
      called automatically by @link(AnApplication.init).
    }
    procedure storeCommandLine; virtual;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the application instance.

      This method is called automatically by @code(TObject.free) when it is
      called on an instance of @link(AnApplication).  It frees
      @link(AnApplication.Environment) and @link(AnApplication.Parameters)
      before calling the inherited routine.
    }
    destructor destroy; override;

    { Execute the application.

      This method should be overridden by descendant classes.  In the base
      implementation, it does nothing.

      @return(A value indicating whether the application executed successfully.
      This value may be passed back to the underlying operating system.
      Generally, a return value of zero (0) means no errors occurred; any other
      value indicates some kind of error condition, with the exact meaning
      dependent on the application.)
    }
    function run: integer; virtual;

    { Suspend execution of the application for at least @code(count)
      milliseconds.  Depending on the underlying operating system, the actual
      amount of time that the process is suspended may be greater than
      @code(count).

      This method simply calls @code(SysUtils.sleep).  It is provided for
      convenience.
    }
    procedure sleep(const count: longword); virtual;

    { Retrieve the absolute path of the executable; this does NOT include the
      name of the executable file itself.  To obtain the name of the executable
      file without its path, call @link(AnApplication.executableName).
    }
    function executablePath: string; virtual;

    { Retrieve the name of the executable file that contains the application.
      This is the name of the file without any path information; to obtain the
      full path to the executable, call @link(AnApplication.executablePath).

      To obtain the name of the executable file @italic(without) its extension,
      set @code(withoutExtension) to @true.
    }
    function executableName(const withoutExtension: boolean = false): string;
      virtual;

    { Get the current working directory of the appliction.

      This method simply calls @code(SysUtils.getCurrentDir) and returns the
      result.  It is provided for convenience.
    }
    function currentDirectory: string; virtual;

    { Set the current working directory of the application.

      This method simply calls @code(SysUtils.setCurrentDir).  It is provided
      for convenience.

      @return(The previous value of @link(AnApplication.currentDirectory).)
    }
    function setCurrentDirectory(const newPath: string): string; virtual;

    { Get the name of the directory where temporary files may be stored.
      Depending on the underlying operating system, this directory may be
      specific to the user that is running the application; to obtain the name
      of a temporary directory that is accessible to all users, set
      @code(forAllUsers) to @true.

      This method simply calls @code(SysUtils.getTempDir) and returns the
      result.  Note that the resulting string will contain a path delimiter.
    }
    function temporaryDirectory(const forAllUsers: boolean = false): string;
      virtual;

    { Get the name of the directory where application configuration files may
      be stored.  Depending on the underlying operating system, this directory
      may be specific to the user that is running the application; to obtain the
      name of a temporary directory that is accessible to all users, set
      @code(forAllUsers) to @true.

      This method simply calls @code(SysUtils.getAppConfigDir) and returns the
      result.  Note that the resulting string will contain a path delimiter.
    }
    function configurationDirectory(const forAllUsers: boolean = false): string;
      virtual;

    { Get the name of the directory where data for the current user is kept.

      This method simply calls @code(SysUtils.getUserDir) and returns the
      result.  Note that the resulting string will contain a path delimiter.
    }
    function userDirectory: string; virtual;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the application, suitable for
      printing to a text-based device, such as a console.

      This method overrides the behavior inherted from
      @link(APrintingObject.toString); it returns the value of
      @link(AnApplication.executableName).
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the dictionary of environment variables available
      to the process.  This reference should NOT be freed by the caller; that
      will be done when the @link(AnApplication) instance is destroyed.

      The dictionary reference returned by this method will contain any and all
      environment variables made available to the process.
    }
    function Environment: ADictionary; virtual;

    { Retrieve a reference to the dictionary of command-line parameters
      available to the process.  This reference should NOT be freed by the
      caller; that will be done when the @link(AnApplication) instance is
      destroyed.

      The dictionary reference returned by this method will contain any and all
      parameters passed to the process.  It will NOT contain the name of the
      application itself (the "zeroth" argument); this is instead returned by
      @link(AnApplication.executablePath).
    }
    function Parameters: AStringList; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AnApplication ***************************************************************
// Initializer
function AnApplication.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    result := false;

    // Construct our environment dictionary and command-line parameters list
    MyEnvironment := ADictionary.new;
    MyParameters := AStringList.new;

    if (MyEnvironment = nil) or (MyParameters = nil) then
      exit;

    // Parse the environment
    Self.parseEnvironment;
    // Get the command-line
    Self.storeCommandLine;

    result := true;
  end;
end;

// Destructor
destructor AnApplication.destroy;

begin
  // Free our environment dictionary and list of command-line parameters
  if MyEnvironment <> nil then
    MyEnvironment.free;

  if MyParameters <> nil then
    MyParameters.free;

  inherited destroy;
end;

// Parse the environment
procedure AnApplication.parseEnvironment;

var
  // Loop counter
  i: integer;
  // Stores the value of an environment variable
  variableValue: string;
{$IFDEF WINDOWS}
  // Stores the name of an environment variable
  variableName: string;
  // Stores the position of the delimiter between names and values
  delimiterPos: integer;
{$ENDIF}

begin
  // Loop through each variable
  for i := 1 to SysUtils.getEnvironmentVariableCount do
  begin
    variableValue := SysUtils.getEnvironmentString(i);
{$IFDEF WINDOWS}
    { On Windows, environment strings can begin with an equals sign, which will
      confuse our parser, since it expects the first equals sign to be the
      delimiter.  So we use the LAST position of the equals sign as the place
      to separate our variable name from its value.
    }
    delimiterPos := Charstring.lastPositionOf(dcteDefaultDelimiter,
      variableValue);
    variableName := System.copy(variableValue, 1, delimiterPos);
    variableValue := System.copy(variableValue, delimiterPos +
      System.length(dcteDefaultDelimiter), System.length(variableValue));

    // Add the variable name and value to our environment dictionary
    MyEnvironment.add(variableName, variableValue);
{$ELSE}
    // Add the variable name and value to our environment dictionary
    MyEnvironment.explode(variableValue);
{$ENDIF}
  end;
end;

// Get the list of command-line parameters passed to the application
procedure AnApplication.storeCommandLine;

var
  // Loop counter
  i: integer;

begin
  for i := 1 to System.paramCount do
    MyParameters.push(System.paramStr(i));
end;

// Run the application
function AnApplication.run: integer;

begin
  { Descendant classes should override this. }
  result := 0;
end;

// Sleep for at least the specified number of milliseconds
procedure AnApplication.sleep(const count: longword);

begin
  SysUtils.sleep(count);
end;

// Retrieve the full path to the executable
function AnApplication.executablePath: string;

begin
  // Get the full path to the executable
  result := SysUtils.expandFileName(System.paramStr(0));
  // Now, remove the name of the executable file itself
  result := SysUtils.extractFilePath(result);
end;

// Retrieve the name of the executable
function AnApplication.executableName(const withoutExtension: boolean = false): string;

begin
  result := SysUtils.extractFileName(System.paramStr(0));

  // Strip the extension, if desired
  if withoutExtension then
    result := SysUtils.extractFileName(result);
end;

// Get the current working directory
function AnApplication.currentDirectory: string;

begin
  result := SysUtils.getCurrentDir;
end;

// Set the current working directory
function AnApplication.setCurrentDirectory(const newPath: string): string;

begin
  // Get the current directory
  result := SysUtils.getCurrentDir;

  // Set the new directory
  SysUtils.setCurrentDir(newPath);
end;

// Get the system-defined temporary file directory
function AnApplication.temporaryDirectory(const forAllUsers: boolean = false): string;

begin
  result := SysUtils.getTempDir(forAllUsers);
end;

// Get the system-defined application configuration directory
function AnApplication.configurationDirectory(const forAllUsers: boolean = false): string;

begin
  result := SysUtils.getAppConfigDir(forAllUsers);
end;

// Get the system-defined user data directory for the current user
function AnApplication.userDirectory: string;

begin
  result := SysUtils.getUserDir;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the application
function AnApplication.toString: AnsiString;

begin
  result := Self.executableName;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the dictionary of environment variables
function AnApplication.Environment: ADictionary;

begin
  Result := MyEnvironment;
end;

// Retrieve a reference to the list of command-line parameters
function AnApplication.Parameters: AStringList;

begin
  Result := MyParameters;
end;
{$ENDIF}

