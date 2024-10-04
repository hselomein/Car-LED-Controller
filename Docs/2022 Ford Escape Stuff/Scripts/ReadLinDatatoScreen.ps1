# Add the .NET assembly for SerialPort
Add-Type -AssemblyName System.IO.Ports.SerialPort

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
    # Close the port when done
    $serialPort.Close()
    Write-Host "Port $portName closed."
}