import hashlib
import hmac
import json
import os
from pathlib import Path
from typing import Optional, Tuple


class RollingCodeTransmitter:
    """
    HMAC-based rolling code transmitter compatible with ESP32 implementation.
    Stores key and counter persistently in JSON file.
    """

    HMAC_KEY_SIZE = 32
    ROLLING_CODE_SIZE = 8
    ROLLING_CODE_DIGITS = 6  # 6-digit decimal code for display

    def __init__(self, device_id: str, storage_path: str = "./rolling_codes"):
        """
        Initialize transmitter with device ID and storage location.

        Args:
            device_id: Unique identifier for this transmitter
            storage_path: Directory to store persistent data
        """
        self.device_id = device_id
        self.storage_path = Path(storage_path)
        self.storage_path.mkdir(parents=True, exist_ok=True)

        self.state_file = self.storage_path / f"{device_id}.json"
        self.key: Optional[bytes] = None
        self.counter: int = 0

        self._load_or_create_state()

    def _load_or_create_state(self):
        """Load existing state or create new key and counter."""
        if self.state_file.exists():
            try:
                with open(self.state_file, "r") as f:
                    state = json.load(f)
                    self.key = bytes.fromhex(state["key"])
                    self.counter = state["counter"]
                    print(
                        f"Loaded existing state for {self.device_id}, counter: {self.counter}"
                    )
            except (json.JSONDecodeError, KeyError) as e:
                print(f"Error loading state: {e}, creating new state")
                self._create_new_state()
        else:
            self._create_new_state()

    def _create_new_state(self):
        """Generate new key and initialize counter."""
        self.key = os.urandom(self.HMAC_KEY_SIZE)
        self.counter = 0
        print(f"Generated new HMAC key for {self.device_id}")
        self._save_state()

    def _save_state(self):
        """Save current state to file."""
        state = {
            "device_id": self.device_id,
            "key": self.key.hex(),
            "counter": self.counter,
        }
        with open(self.state_file, "w") as f:
            json.dump(state, f, indent=2)

    def generate_code(self, counter: int) -> bytes:
        """
        Generate HMAC rolling code for specific counter value.

        Args:
            counter: Counter value to use

        Returns:
            8-byte rolling code
        """
        # Convert counter to 8-byte big-endian
        counter_bytes = counter.to_bytes(8, byteorder="big")

        # Generate HMAC-SHA256
        h = hmac.new(self.key, counter_bytes, hashlib.sha256)
        hmac_result = h.digest()

        # Return first 8 bytes
        return hmac_result[: self.ROLLING_CODE_SIZE]

    def code_to_digits(self, code: bytes) -> int:
        """
        Convert code bytes to 6-digit decimal number.

        Args:
            code: Rolling code bytes

        Returns:
            6-digit integer (0-999999)
        """
        # Use first 4 bytes as integer
        value = int.from_bytes(code[:4], byteorder="big")
        # Modulo to get 6 digits
        return value % 1000000

    def generate_code_digits(self, counter: int) -> int:
        """
        Generate rolling code as 6-digit number.

        Args:
            counter: Counter value to use

        Returns:
            6-digit code
        """
        code = self.generate_code(counter)
        return self.code_to_digits(code)

    def next_code(self) -> Tuple[int, bytes, int]:
        """
        Generate next rolling code and increment counter.

        Returns:
            Tuple of (counter, code_bytes, code_digits)
        """
        code = self.generate_code(self.counter)
        code_digits = self.code_to_digits(code)
        current_counter = self.counter
        self.counter += 1
        self._save_state()

        return current_counter, code, code_digits

    def get_key_hex(self) -> str:
        """Get HMAC key in hex format for sharing with receiver."""
        return self.key.hex()

    def export_for_receiver(self) -> dict:
        """
        Export configuration for receiver/ESP32.

        Returns:
            Dictionary with key and current counter
        """
        return {
            "device_id": self.device_id,
            "hmac_key": self.get_key_hex(),
            "counter": self.counter,
            "key_size": self.HMAC_KEY_SIZE,
            "code_size": self.ROLLING_CODE_SIZE,
        }

    def create_transmission_packet(self) -> dict:
        """
        Create a complete transmission packet with counter and code.
        Ready to be serialized and sent to receiver.

        Returns:
            Dictionary with device_id, counter, code (hex), and code_digits
        """
        counter, code, code_digits = self.next_code()
        return {
            "device_id": self.device_id,
            "counter": counter,
            "code": code.hex(),
            "code_digits": f"{code_digits:06d}",  # 6-digit formatted string
        }

    def reset_counter(self, new_counter: int = 0):
        """Reset counter to specific value (use with caution)."""
        self.counter = new_counter
        self._save_state()
        print(f"Counter reset to {new_counter}")


def demo():
    """Demonstration of transmitter functionality."""
    print("=" * 60)
    print("HMAC Rolling Code Transmitter")
    print("=" * 60)

    # Create transmitter
    tx = RollingCodeTransmitter("TX_001")

    print(f"\nDevice ID: {tx.device_id}")
    print(f"HMAC Key: {tx.get_key_hex()}")
    print(f"Current Counter: {tx.counter}")

    # Generate some codes
    print("\n--- Generating 1 Rolling Codes (6-digit format) ---")
    for i in range(1):
        counter, code_bytes, code_digits = tx.next_code()
        print(
            f"Counter: {counter:4d} | Hex: {code_bytes.hex().upper()} | Digits: {code_digits:06d}"
        )

    # Show export format
    print("\n--- Export for ESP32 Receiver ---")
    config = tx.export_for_receiver()
    print(json.dumps(config, indent=2))

    # Create transmission packets
    # print("\n--- Sample Transmission Packets (for Django/API) ---")
    # for i in range(3):
    #     packet = tx.create_transmission_packet()
    #     print(json.dumps(packet, indent=2))
    #     print(f"   → User enters: {packet['code_digits']}")

    print("\n" + "=" * 60)
    print("Demo Complete")
    print("=" * 60)


import sys

if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1].isdigit():
        tx = RollingCodeTransmitter("TX_001")

        print(f"\nDevice ID: {tx.device_id}")
        print(f"HMAC Key: {tx.get_key_hex()}")

        tx.counter = int(sys.argv[1])

        counter, code_bytes, code_digits = tx.next_code()
        print(
            f"Counter: {counter:4d} | Hex: {code_bytes.hex().upper()} | Digits: {code_digits:06d}"
        )

    else:
        tx = RollingCodeTransmitter("TX_001")

        print(f"\nDevice ID: {tx.device_id}")
        print(f"HMAC Key: {tx.get_key_hex()}")

        tx.counter += 1

        counter, code_bytes, code_digits = tx.next_code()
        print(
            f"Counter: {counter:4d} | Hex: {code_bytes.hex().upper()} | Digits: {code_digits:06d}"
        )
