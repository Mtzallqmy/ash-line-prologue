import React from 'react';
import { SafeAreaView, StyleSheet, Text, View } from 'react-native';
import GameButton from '../components/GameButton';
import { colors } from '../theme';

export default function MenuScreen({ onPlay, onArmory }) {
  return (
    <SafeAreaView style={styles.root}>
      <View style={styles.hero}>
        <Text style={styles.eyebrow}>TACTICAL MOBILE PROTOTYPE</Text>
        <Text style={styles.title}>ASH LINE</Text>
        <Text style={styles.subtitle}>PROLOGUE · NAMAR TRAINING DISTRICT</Text>
        <Text style={styles.body}>
          نسخة Expo مستقلة قابلة للبناء على Android. ثلاث مراحل تدريبية، ثلاثة أسلحة، AI خفيف، درون استطلاع واستخراج.
        </Text>
        <View style={styles.actions}>
          <GameButton label="ابدأ المهمة" onPress={onPlay} />
          <GameButton label="العتاد" onPress={onArmory} />
        </View>
        <Text style={styles.footer}>v0.0.1 · Android 8+ · ARM64</Text>
      </View>
      <View style={styles.sidePanel}>
        <Text style={styles.panelTitle}>العملية</Text>
        <Text style={styles.line}>01  الوصول إلى نقطة التجمع</Text>
        <Text style={styles.line}>02  أول اشتباك</Text>
        <Text style={styles.line}>03  عين في السماء</Text>
        <View style={styles.divider} />
        <Text style={styles.muted}>المدينة: نمار</Text>
        <Text style={styles.muted}>الحالة: تدريب قتالي</Text>
        <Text style={styles.muted}>المنظور: تكتيكي علوي</Text>
      </View>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  root: { flex: 1, flexDirection: 'row', backgroundColor: colors.bg, padding: 28, gap: 22 },
  hero: { flex: 1.6, justifyContent: 'center', paddingHorizontal: 26 },
  eyebrow: { color: colors.accent, fontSize: 13, fontWeight: '900', letterSpacing: 2.4 },
  title: { color: colors.text, fontSize: 64, lineHeight: 66, fontWeight: '900', letterSpacing: 4, marginTop: 8 },
  subtitle: { color: colors.muted, fontSize: 16, fontWeight: '700', letterSpacing: 1.1, marginTop: 4 },
  body: { color: '#cbd5e1', maxWidth: 620, fontSize: 17, lineHeight: 27, marginTop: 20, textAlign: 'left' },
  actions: { flexDirection: 'row', gap: 12, marginTop: 28 },
  footer: { color: '#64748b', marginTop: 24, fontSize: 12 },
  sidePanel: { flex: 0.8, backgroundColor: colors.panel, borderRadius: 22, padding: 24, justifyContent: 'center', borderWidth: 1, borderColor: '#334155' },
  panelTitle: { color: colors.text, fontSize: 23, fontWeight: '900', marginBottom: 18 },
  line: { color: '#e2e8f0', fontSize: 16, marginVertical: 7 },
  divider: { height: 1, backgroundColor: '#334155', marginVertical: 18 },
  muted: { color: colors.muted, fontSize: 14, marginVertical: 4 },
});
