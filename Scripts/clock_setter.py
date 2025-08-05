#!/usr/bin/env python3
"""
PC Time Sender for STM32 RTC Sync
Sends current PC time to STM32 and displays verification
"""

import serial
import time
import datetime
import sys


def find_stm32_port():
    """Find STM32 COM port automatically"""
    import serial.tools.list_ports

    for port in serial.tools.list_ports.comports():
        if "FTDI" in port.manufacturer:
            return port.device
    return None


def sync_rtc_time(port, baudrate=9600):
    """Send PC time to STM32 and verify"""
    try:
        # Open serial connection
        ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(1)  # Wait for connection

        print(f"Connected to {port}")

        # Get current PC time
        now = datetime.datetime.now()
        time_cmd = f"SYNC:{now.strftime('%Y-%m-%d:%H:%M:%S')}"
        print(f"Sending: {time_cmd}")
        ser.write(time_cmd.encode() + b'\r\n')
        ser.flush()
        time.sleep(1)
        # Read response
        print("\nSTM32 Response:")
        for _ in range(5):  # Read multiple lines
            response = ser.readline().decode().strip()
            if response:
                print(response)

        ser.close()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


def manual_check_time(port, baudrate=9600):
    """Check current RTC time"""
    try:
        ser = serial.Serial(port, baudrate, timeout=2)
        time.sleep(1)

        for i in range(3):
            ser.write(b"CHECK\r\n")
            response = ser.readline().decode().strip()
            print(f"RTC Time: {response}")
            time.sleep(0.1)  # Small delay after write

        ser.close()

    except Exception as e:
        print(f"Error: {e}")


def monitor_uart(port, baudrate=9600):
    """Monitor UART output continuously"""
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Monitoring UART on {port} (Ctrl+C to stop)")

        while True:
            if ser.in_waiting > 0:
                response = ser.readline().decode().strip()
                if response:
                    print(f"{datetime.datetime.now().strftime('%H:%M:%S')} - {response}")

    except KeyboardInterrupt:
        print("\nMonitoring stopped")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        ser.close()

def main():
    # Auto-detect STM32 port
    port = find_stm32_port()

    if not port:
        print("STM32 not found. Available ports:")
        import serial.tools.list_ports
        for p in serial.tools.list_ports.comports():
            print(f"  {p.device} - {p.description}")

        port = input("Enter COM port manually (e.g., COM3): ")

    print(f"Using port: {port}")

    while True:
        print("\n1. Sync PC time to RTC")
        print("2. Check RTC time")
        print("3. Auto-sync every 30 seconds")
        print("4. Monitor UART output")
        print("q. Exit")

        choice = input("Choice: ")

        if choice == '1':
            sync_rtc_time(port)
        elif choice == '2':
            manual_check_time(port)
        elif choice == '3':
            print("Auto-sync mode (Ctrl+C to stop)")
            try:
                while True:
                    sync_rtc_time(port)
                    time.sleep(0.2)
            except KeyboardInterrupt:
                print("\nAuto-sync stopped")
        elif choice == '4':
            monitor_uart(port)
        elif choice.lower() == 'q':
            break



if __name__ == "__main__":
    main()