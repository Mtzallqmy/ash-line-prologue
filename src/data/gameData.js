export const WORLD = { width: 1000, height: 600 };

export const WEAPONS = [
  {
    id: 'WPN_AR_001',
    name: 'AR-24',
    type: 'Assault Rifle',
    damage: 28,
    magazineSize: 30,
    reserve: 90,
    rpm: 650,
    spread: 0.035,
  },
  {
    id: 'WPN_SMG_001',
    name: 'Viper-9',
    type: 'SMG',
    damage: 21,
    magazineSize: 32,
    reserve: 96,
    rpm: 850,
    spread: 0.065,
  },
  {
    id: 'WPN_PST_001',
    name: 'P-12',
    type: 'Pistol',
    damage: 32,
    magazineSize: 15,
    reserve: 45,
    rpm: 350,
    spread: 0.025,
  },
];

export const MISSIONS = [
  {
    id: 'arrival',
    title: 'المهمة 01 — الوصول',
    objective: 'تحرك إلى نقطة التجمع الخضراء.',
  },
  {
    id: 'first-contact',
    title: 'المهمة 02 — أول اشتباك',
    objective: 'حيّد أربعة أهداف مع استخدام الغطاء وإعادة التعبئة.',
  },
  {
    id: 'eye-above',
    title: 'المهمة 03 — عين في السماء',
    objective: 'استخدم المسح بالدرون، حيّد الأهداف ثم اتجه إلى الاستخراج.',
  },
];

export const COVERS = [
  { x: 305, y: 95, w: 90, h: 34 },
  { x: 590, y: 80, w: 45, h: 130 },
  { x: 235, y: 340, w: 55, h: 145 },
  { x: 470, y: 300, w: 115, h: 42 },
  { x: 730, y: 390, w: 95, h: 38 },
  { x: 785, y: 160, w: 45, h: 115 },
];

export function createAmmoState() {
  return Object.fromEntries(
    WEAPONS.map((weapon) => [
      weapon.id,
      { magazine: weapon.magazineSize, reserve: weapon.reserve },
    ])
  );
}

export function spawnEnemiesForMission(missionIndex) {
  if (missionIndex === 1) {
    return [
      { id: 'E1', x: 720, y: 120, health: 100, cooldown: 600, dormant: false, markedUntil: Infinity },
      { id: 'E2', x: 820, y: 315, health: 100, cooldown: 850, dormant: false, markedUntil: Infinity },
      { id: 'E3', x: 590, y: 475, health: 100, cooldown: 1100, dormant: false, markedUntil: Infinity },
      { id: 'E4', x: 390, y: 185, health: 100, cooldown: 1300, dormant: false, markedUntil: Infinity },
    ];
  }
  if (missionIndex === 2) {
    return [
      { id: 'D1', x: 835, y: 105, health: 100, cooldown: 900, dormant: true, markedUntil: 0 },
      { id: 'D2', x: 735, y: 315, health: 100, cooldown: 1100, dormant: true, markedUntil: 0 },
      { id: 'D3', x: 585, y: 510, health: 100, cooldown: 1250, dormant: true, markedUntil: 0 },
    ];
  }
  return [];
}
