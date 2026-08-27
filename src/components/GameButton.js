import React from 'react';
import { Pressable, StyleSheet, Text } from 'react-native';
import { colors } from '../theme';

export default function GameButton({ label, onPress, onPressIn, onPressOut, compact = false, danger = false, disabled = false }) {
  return (
    <Pressable
      accessibilityRole="button"
      disabled={disabled}
      onPress={onPress}
      onPressIn={onPressIn}
      onPressOut={onPressOut}
      style={({ pressed }) => [
        styles.button,
        compact && styles.compact,
        danger && styles.danger,
        disabled && styles.disabled,
        pressed && !disabled && styles.pressed,
      ]}
    >
      <Text style={[styles.label, compact && styles.compactLabel]}>{label}</Text>
    </Pressable>
  );
}

const styles = StyleSheet.create({
  button: {
    minWidth: 116,
    minHeight: 48,
    paddingHorizontal: 18,
    paddingVertical: 12,
    borderRadius: 14,
    borderWidth: 1,
    borderColor: '#f59e0b88',
    backgroundColor: '#78350fcc',
    alignItems: 'center',
    justifyContent: 'center',
  },
  compact: {
    minWidth: 56,
    minHeight: 44,
    paddingHorizontal: 10,
    paddingVertical: 8,
    borderRadius: 12,
  },
  danger: {
    backgroundColor: '#7f1d1dcc',
    borderColor: '#ef444488',
  },
  disabled: { opacity: 0.42 },
  pressed: { transform: [{ scale: 0.96 }], backgroundColor: colors.accentSoft },
  label: { color: colors.text, fontSize: 16, fontWeight: '800' },
  compactLabel: { fontSize: 18 },
});
