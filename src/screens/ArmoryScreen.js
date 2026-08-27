import React from 'react';
import { SafeAreaView, StyleSheet, Text, View } from 'react-native';
import GameButton from '../components/GameButton';
import { WEAPONS } from '../data/gameData';
import { colors } from '../theme';

export default function ArmoryScreen({ onBack }) {
  return (
    <SafeAreaView style={styles.root}>
      <View style={styles.header}>
        <View>
          <Text style={styles.kicker}>ASH LINE / ARMORY</Text>
          <Text style={styles.title}>العتاد الأساسي</Text>
        </View>
        <GameButton label="رجوع" onPress={onBack} />
      </View>
      <View style={styles.grid}>
        {WEAPONS.map((weapon) => (
          <View key={weapon.id} style={styles.card}>
            <Text style={styles.weapon}>{weapon.name}</Text>
            <Text style={styles.type}>{weapon.type}</Text>
            <View style={styles.stat}><Text style={styles.label}>الضرر</Text><Text style={styles.value}>{weapon.damage}</Text></View>
            <View style={styles.stat}><Text style={styles.label}>المخزن</Text><Text style={styles.value}>{weapon.magazineSize}</Text></View>
            <View style={styles.stat}><Text style={styles.label}>الاحتياطي</Text><Text style={styles.value}>{weapon.reserve}</Text></View>
            <View style={styles.stat}><Text style={styles.label}>RPM</Text><Text style={styles.value}>{weapon.rpm}</Text></View>
            <Text style={styles.id}>{weapon.id}</Text>
          </View>
        ))}
      </View>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  root: { flex: 1, backgroundColor: colors.bg, padding: 24 },
  header: { flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between', marginBottom: 18 },
  kicker: { color: colors.accent, fontSize: 12, fontWeight: '900', letterSpacing: 2 },
  title: { color: colors.text, fontSize: 32, fontWeight: '900', marginTop: 4 },
  grid: { flex: 1, flexDirection: 'row', gap: 16 },
  card: { flex: 1, backgroundColor: colors.panel, borderRadius: 20, padding: 22, borderWidth: 1, borderColor: '#334155' },
  weapon: { color: colors.text, fontSize: 28, fontWeight: '900' },
  type: { color: colors.muted, fontSize: 14, marginTop: 4, marginBottom: 18 },
  stat: { flexDirection: 'row', justifyContent: 'space-between', paddingVertical: 9, borderBottomWidth: 1, borderBottomColor: '#263244' },
  label: { color: colors.muted, fontSize: 14 },
  value: { color: colors.text, fontSize: 16, fontWeight: '800' },
  id: { marginTop: 'auto', color: '#64748b', fontSize: 12, fontFamily: 'monospace' },
});
