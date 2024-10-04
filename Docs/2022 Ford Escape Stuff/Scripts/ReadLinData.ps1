# Set COM Port parameters
$portName = "COM3"
$baudRate = 10417
$dataBits = 8
$parity = [System.IO.Ports.Parity]::None
$stopBits = [System.IO.Ports.StopBits]::One

# Create a new SerialPort object
$serialPort = New-Object System.IO.Ports.SerialPort $portName, $baudRate, $parity, $dataBits, $stopBits

# Set read timeout
$serialPort.ReadTimeout = 5000 # in milliseconds

# Open the Serial Port
try {
    $serialPort.Open()
    Write-Host "Connected to $portName at $baudRate baud."
} catch {
    Write-Host "Failed to open port: $_"
    exit
}

# Generate a new filename with a timestamp
$timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$filePath = "C:\Users\hselo\OneDrive\Documents\GitHub Repo\Car LED Controller Project\Docs\2022 Ford Escape Stuff\Scripts\Dumps\outputfile_$timestamp.txt"

# Create a FileStream and StreamWriter to write data to a file
try {
    $fileStream = [System.IO.File]::OpenWrite($filePath)
    $streamWriter = New-Object System.IO.StreamWriter($fileStream)
    Write-Host "File stream created at $filePath"
} catch {
    Write-Host "Failed to create file: $_"
    $serialPort.Close()
    exit
}

# Record data for 5 seconds
$startTime = Get-Date
$endTime = $startTime.AddSeconds(5)
Write-Host "Starting data recording for 5 seconds..."

try {
    while ((Get-Date) -lt $endTime) {
        try {
            # Read a line from the serial port
            Write-Host "Attempting to read from serial port..."
            $line = $serialPort.ReadLine()
            
            # Display the line in the console
            Write-Host "Read line: $line"
            
            # Write the line to the file
            $streamWriter.WriteLine($line)
        } catch [System.TimeoutException] {
            Write-Host "Read timeout occurred, continuing..."
        } catch {
            Write-Host "Error reading from COM port: $_"
            break
        }
    }
} catch {
    Write-Host "Unexpected error: $_"
} finally {
    # Close the port and file stream when done
    $serialPort.Close()
    $streamWriter.Close()
    $fileStream.Close()
    Write-Host "Port $portName closed and data written to $filePath."
}