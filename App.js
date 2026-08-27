import React, { useState } from 'react';
import { StatusBar } from 'expo-status-bar';
import { View } from 'react-native';
import ArmoryScreen from './src/screens/ArmoryScreen';
import GameScreen from './src/game/GameScreen';
import MenuScreen from './src/screens/MenuScreen';
import { colors } from './src/theme';

export default function App() {
  const [screen, setScreen] = useState('menu');

  return (
    <View style={{ flex: 1, backgroundColor: colors.bg }}>
      <StatusBar style="light" hidden />
      {screen === 'menu' && <MenuScreen onPlay={() => setScreen('game')} onArmory={() => setScreen('armory')} />}
      {screen === 'armory' && <ArmoryScreen onBack={() => setScreen('menu')} />}
      {screen === 'game' && <GameScreen onExit={() => setScreen('menu')} />}
    </View>
  );
}
