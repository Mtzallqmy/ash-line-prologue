import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Pressable, SafeAreaView, StyleSheet, Text, useWindowDimensions, View } from 'react-native';
import GameButton from '../components/GameButton';
import { COVERS, createAmmoState, MISSIONS, spawnEnemiesForMission, WEAPONS, WORLD } from '../data/gameData';
import { colors } from '../theme';

const PLAYER_RADIUS = 16;
const ENEMY_RADIUS = 17;
const PLAYER_SPEED = 205;
const ENEMY_SPEED = 66;
const BULLET_SPEED = 700;
const ENEMY_BULLET_SPEED = 360;

const clamp = (value, min, max) => Math.max(min, Math.min(max, value));
const length = (x, y) => Math.sqrt(x * x + y * y) || 1;
const distance = (a, b) => length(a.x - b.x, a.y - b.y);

function pointInRect(point, rect) {
  return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

function normalize(x, y) {
  const d = length(x, y);
  return { x: x / d, y: y / d };
}

function initialWorld() {
  return {
    player: { x: 115, y: 500, health: 100 },
    missionIndex: 0,
    enemies: [],
    bullets: [],
    enemyBullets: [],
    ammo: createAmmoState(),
    weaponIndex: 0,
    lastPlayerShot: 0,
    reloadingUntil: 0,
    extractionUnlocked: false,
    completed: false,
    failed: false,
    droneCooldownUntil: 0,
    message: 'تحرك إلى نقطة التجمع.',
    score: 0,
  };
}

export default function GameScreen({ onExit }) {
  const { width, height } = useWindowDimensions();
  const [frame, setFrame] = useState(0);
  const worldRef = useRef(initialWorld());
  const moveRef = useRef({ up: false, down: false, left: false, right: false });
  const aimRef = useRef({ x: 1, y: 0 });
  const fireHeldRef = useRef(false);
  const lastTickRef = useRef(Date.now());

  const ui = worldRef.current;
  const mission = MISSIONS[ui.missionIndex];
  const currentWeapon = WEAPONS[ui.weaponIndex];
  const currentAmmo = ui.ammo[currentWeapon.id];

  const arena = useMemo(() => {
    const controlReserve = Math.max(205, width * 0.22);
    const availableWidth = width - controlReserve * 2;
    const availableHeight = height - 118;
    const scale = Math.min(availableWidth / WORLD.width, availableHeight / WORLD.height);
    return {
      scale,
      left: (width - WORLD.width * scale) / 2,
      top: 82 + Math.max(0, (availableHeight - WORLD.height * scale) / 2),
      width: WORLD.width * scale,
      height: WORLD.height * scale,
    };
  }, [width, height]);

  const renderEntityStyle = useCallback((x, y, size) => ({
    left: arena.left + (x - size / 2) * arena.scale,
    top: arena.top + (y - size / 2) * arena.scale,
    width: size * arena.scale,
    height: size * arena.scale,
    borderRadius: size * arena.scale,
  }), [arena]);

  const advanceMission = useCallback((nextIndex) => {
    const world = worldRef.current;
    world.missionIndex = nextIndex;
    world.player.x = nextIndex === 1 ? 155 : 120;
    world.player.y = nextIndex === 1 ? 480 : 510;
    world.bullets = [];
    world.enemyBullets = [];
    world.enemies = spawnEnemiesForMission(nextIndex);
    world.extractionUnlocked = false;
    world.message = MISSIONS[nextIndex].objective;
  }, []);

  const restart = useCallback(() => {
    worldRef.current = initialWorld();
    moveRef.current = { up: false, down: false, left: false, right: false };
    aimRef.current = { x: 1, y: 0 };
    fireHeldRef.current = false;
    lastTickRef.current = Date.now();
    setFrame((v) => v + 1);
  }, []);

  const reload = useCallback(() => {
    const world = worldRef.current;
    if (world.failed || world.completed) return;
    const weapon = WEAPONS[world.weaponIndex];
    const ammo = world.ammo[weapon.id];
    if (ammo.magazine >= weapon.magazineSize || ammo.reserve <= 0 || Date.now() < world.reloadingUntil) return;
    world.reloadingUntil = Date.now() + 900;
    world.message = 'إعادة تعبئة...';
    setFrame((v) => v + 1);
  }, []);

  const switchWeapon = useCallback(() => {
    const world = worldRef.current;
    if (Date.now() < world.reloadingUntil) return;
    world.weaponIndex = (world.weaponIndex + 1) % WEAPONS.length;
    world.message = `تم اختيار ${WEAPONS[world.weaponIndex].name}`;
    setFrame((v) => v + 1);
  }, []);

  const droneScan = useCallback(() => {
    const world = worldRef.current;
    const now = Date.now();
    if (world.missionIndex !== 2 || world.droneCooldownUntil > now || world.failed || world.completed) return;
    world.droneCooldownUntil = now + 7000;
    world.enemies.forEach((enemy) => {
      enemy.dormant = false;
      enemy.markedUntil = now + 10000;
    });
    world.message = 'تم تحديد الأهداف. نافذة المسح 10 ثوانٍ.';
    setFrame((v) => v + 1);
  }, []);

  const tick = useCallback(() => {
    const now = Date.now();
    const world = worldRef.current;
    const dt = Math.min(0.05, Math.max(0.008, (now - lastTickRef.current) / 1000));
    lastTickRef.current = now;
    if (world.failed || world.completed) {
      setFrame((v) => v + 1);
      return;
    }

    if (world.reloadingUntil > 0 && now >= world.reloadingUntil) {
      const weapon = WEAPONS[world.weaponIndex];
      const ammo = world.ammo[weapon.id];
      const needed = weapon.magazineSize - ammo.magazine;
      const moved = Math.min(needed, ammo.reserve);
      ammo.magazine += moved;
      ammo.reserve -= moved;
      world.reloadingUntil = 0;
      world.message = mission.objective;
    }

    const movement = moveRef.current;
    let mx = (movement.right ? 1 : 0) - (movement.left ? 1 : 0);
    let my = (movement.down ? 1 : 0) - (movement.up ? 1 : 0);
    if (mx || my) {
      const dir = normalize(mx, my);
      world.player.x = clamp(world.player.x + dir.x * PLAYER_SPEED * dt, PLAYER_RADIUS, WORLD.width - PLAYER_RADIUS);
      world.player.y = clamp(world.player.y + dir.y * PLAYER_SPEED * dt, PLAYER_RADIUS, WORLD.height - PLAYER_RADIUS);
    }

    if (world.missionIndex === 0 && world.player.x > 855 && world.player.y < 165) {
      advanceMission(1);
    }

    const weapon = WEAPONS[world.weaponIndex];
    const ammo = world.ammo[weapon.id];
    const secondsPerShot = 60 / weapon.rpm;
    if (fireHeldRef.current && now >= world.reloadingUntil && ammo.magazine > 0 && (now - world.lastPlayerShot) / 1000 >= secondsPerShot) {
      const aim = normalize(aimRef.current.x, aimRef.current.y);
      const noise = (Math.random() - 0.5) * weapon.spread;
      const cs = Math.cos(noise);
      const sn = Math.sin(noise);
      const shot = { x: aim.x * cs - aim.y * sn, y: aim.x * sn + aim.y * cs };
      world.bullets.push({ x: world.player.x, y: world.player.y, vx: shot.x * BULLET_SPEED, vy: shot.y * BULLET_SPEED, damage: weapon.damage, life: 1.25 });
      ammo.magazine -= 1;
      world.lastPlayerShot = now;
    }

    if (fireHeldRef.current && ammo.magazine === 0 && ammo.reserve > 0 && world.reloadingUntil === 0) {
      world.message = 'المخزن فارغ — أعد التعبئة.';
    }

    world.bullets = world.bullets.filter((bullet) => {
      bullet.x += bullet.vx * dt;
      bullet.y += bullet.vy * dt;
      bullet.life -= dt;
      if (bullet.life <= 0 || bullet.x < 0 || bullet.x > WORLD.width || bullet.y < 0 || bullet.y > WORLD.height) return false;
      if (COVERS.some((cover) => pointInRect(bullet, cover))) return false;
      for (const enemy of world.enemies) {
        if (enemy.health > 0 && distance(bullet, enemy) <= ENEMY_RADIUS + 5) {
          enemy.health -= bullet.damage;
          if (enemy.health <= 0) {
            world.score += 1;
            world.message = 'هدف محيّد.';
          }
          return false;
        }
      }
      return true;
    });

    world.enemies.forEach((enemy) => {
      if (enemy.health <= 0 || enemy.dormant) return;
      const dx = world.player.x - enemy.x;
      const dy = world.player.y - enemy.y;
      const dist = length(dx, dy);
      const dir = normalize(dx, dy);
      if (dist > 240) {
        enemy.x = clamp(enemy.x + dir.x * ENEMY_SPEED * dt, ENEMY_RADIUS, WORLD.width - ENEMY_RADIUS);
        enemy.y = clamp(enemy.y + dir.y * ENEMY_SPEED * dt, ENEMY_RADIUS, WORLD.height - ENEMY_RADIUS);
      }
      enemy.cooldown -= dt * 1000;
      if (dist < 490 && enemy.cooldown <= 0) {
        const error = (Math.random() - 0.5) * 0.22;
        const cs = Math.cos(error);
        const sn = Math.sin(error);
        const ex = dir.x * cs - dir.y * sn;
        const ey = dir.x * sn + dir.y * cs;
        world.enemyBullets.push({ x: enemy.x, y: enemy.y, vx: ex * ENEMY_BULLET_SPEED, vy: ey * ENEMY_BULLET_SPEED, life: 1.7, damage: 12 });
        enemy.cooldown = 850 + Math.random() * 650;
      }
    });

    world.enemyBullets = world.enemyBullets.filter((bullet) => {
      bullet.x += bullet.vx * dt;
      bullet.y += bullet.vy * dt;
      bullet.life -= dt;
      if (bullet.life <= 0 || bullet.x < 0 || bullet.x > WORLD.width || bullet.y < 0 || bullet.y > WORLD.height) return false;
      if (COVERS.some((cover) => pointInRect(bullet, cover))) return false;
      if (distance(bullet, world.player) <= PLAYER_RADIUS + 5) {
        world.player.health = Math.max(0, world.player.health - bullet.damage);
        if (world.player.health <= 0) {
          world.failed = true;
          world.message = 'فشلت المهمة.';
        }
        return false;
      }
      return true;
    });

    const living = world.enemies.filter((enemy) => enemy.health > 0);
    if (world.missionIndex === 1 && world.enemies.length > 0 && living.length === 0) {
      advanceMission(2);
    } else if (world.missionIndex === 2 && world.enemies.length > 0 && living.length === 0) {
      world.extractionUnlocked = true;
      world.message = 'منطقة الاستخراج مفتوحة.';
      world.enemies = [];
    }

    if (world.missionIndex === 2 && world.extractionUnlocked && world.player.x > 855 && world.player.y > 455) {
      world.completed = true;
      world.message = 'PROLOGUE COMPLETE';
    }

    setFrame((v) => (v + 1) % 100000);
  }, [advanceMission, mission.objective]);

  useEffect(() => {
    const timer = setInterval(tick, 33);
    return () => clearInterval(timer);
  }, [tick]);

  const setMove = (key, value) => { moveRef.current[key] = value; };
  const setAim = (x, y) => { aimRef.current = { x, y }; };
  const healthPercent = clamp(ui.player.health / 100, 0, 1);
  const livingEnemies = ui.enemies.filter((enemy) => enemy.health > 0).length;
  const now = Date.now();

  return (
    <SafeAreaView style={styles.root}>
      <View style={styles.topBar}>
        <View style={styles.missionBlock}>
          <Text style={styles.missionTitle}>{mission.title}</Text>
          <Text style={styles.objective}>{ui.message}</Text>
        </View>
        <View style={styles.statusBlock}>
          <Text style={styles.statusText}>HP {Math.ceil(ui.player.health)}</Text>
          <View style={styles.healthTrack}><View style={[styles.healthFill, { width: `${healthPercent * 100}%` }]} /></View>
        </View>
        <View style={styles.weaponBlock}>
          <Text style={styles.weaponName}>{currentWeapon.name}</Text>
          <Text style={styles.ammoText}>{currentAmmo.magazine} / {currentAmmo.reserve}</Text>
          <Text style={styles.smallText}>أهداف: {livingEnemies} · نقاط: {ui.score}</Text>
        </View>
      </View>

      <View pointerEvents="none" style={[styles.arena, { left: arena.left, top: arena.top, width: arena.width, height: arena.height }]}>
        <View style={styles.gridLineA} />
        <View style={styles.gridLineB} />
      </View>

      {COVERS.map((cover, index) => (
        <View key={`cover-${index}`} pointerEvents="none" style={[styles.cover, {
          left: arena.left + cover.x * arena.scale,
          top: arena.top + cover.y * arena.scale,
          width: cover.w * arena.scale,
          height: cover.h * arena.scale,
        }]} />
      ))}

      {ui.missionIndex === 0 && (
        <View pointerEvents="none" style={[styles.waypoint, renderEntityStyle(900, 100, 54)]}><Text style={styles.zoneText}>A</Text></View>
      )}
      {ui.missionIndex === 2 && ui.extractionUnlocked && (
        <View pointerEvents="none" style={[styles.extraction, renderEntityStyle(905, 505, 68)]}><Text style={styles.zoneText}>EX</Text></View>
      )}

      <View pointerEvents="none" style={[styles.player, renderEntityStyle(ui.player.x, ui.player.y, PLAYER_RADIUS * 2)]} />
      <View pointerEvents="none" style={[styles.aimLine, {
        left: arena.left + (ui.player.x + aimRef.current.x * 26) * arena.scale,
        top: arena.top + (ui.player.y + aimRef.current.y * 26) * arena.scale,
        width: 7 * arena.scale,
        height: 7 * arena.scale,
      }]} />

      {ui.enemies.filter((enemy) => enemy.health > 0).map((enemy) => {
        const visible = ui.missionIndex !== 2 || enemy.markedUntil > now;
        if (!visible) return null;
        return (
          <View key={enemy.id} pointerEvents="none" style={[styles.enemy, renderEntityStyle(enemy.x, enemy.y, ENEMY_RADIUS * 2), enemy.markedUntil > now && styles.markedEnemy]}>
            <View style={[styles.enemyHealth, { width: `${clamp(enemy.health / 100, 0, 1) * 100}%` }]} />
          </View>
        );
      })}

      {ui.bullets.map((bullet, index) => (
        <View key={`b-${index}-${frame}`} pointerEvents="none" style={[styles.bullet, renderEntityStyle(bullet.x, bullet.y, 8)]} />
      ))}
      {ui.enemyBullets.map((bullet, index) => (
        <View key={`eb-${index}-${frame}`} pointerEvents="none" style={[styles.enemyBullet, renderEntityStyle(bullet.x, bullet.y, 8)]} />
      ))}

      <View style={styles.leftControls}>
        <GameButton compact label="▲" onPressIn={() => setMove('up', true)} onPressOut={() => setMove('up', false)} />
        <View style={styles.controlRow}>
          <GameButton compact label="◀" onPressIn={() => setMove('left', true)} onPressOut={() => setMove('left', false)} />
          <GameButton compact label="▶" onPressIn={() => setMove('right', true)} onPressOut={() => setMove('right', false)} />
        </View>
        <GameButton compact label="▼" onPressIn={() => setMove('down', true)} onPressOut={() => setMove('down', false)} />
      </View>

      <View style={styles.rightControls}>
        <View style={styles.aimPad}>
          <GameButton compact label="⌃" onPress={() => setAim(0, -1)} />
          <View style={styles.controlRow}>
            <GameButton compact label="‹" onPress={() => setAim(-1, 0)} />
            <GameButton compact label="›" onPress={() => setAim(1, 0)} />
          </View>
          <GameButton compact label="⌄" onPress={() => setAim(0, 1)} />
        </View>
        <View style={styles.actionRow}>
          <Pressable
            onPressIn={() => { fireHeldRef.current = true; }}
            onPressOut={() => { fireHeldRef.current = false; }}
            style={({ pressed }) => [styles.fireButton, pressed && styles.firePressed]}
          >
            <Text style={styles.fireText}>FIRE</Text>
          </Pressable>
          <View style={styles.utilityColumn}>
            <GameButton compact label="R" onPress={reload} />
            <GameButton compact label="↻" onPress={switchWeapon} />
            {ui.missionIndex === 2 && <GameButton compact label="DRN" onPress={droneScan} disabled={ui.droneCooldownUntil > now} />}
          </View>
        </View>
      </View>

      <View style={styles.quitButton}><GameButton compact label="✕" danger onPress={onExit} /></View>

      {(ui.failed || ui.completed) && (
        <View style={styles.overlay}>
          <View style={styles.modal}>
            <Text style={[styles.modalTitle, ui.completed && { color: colors.success }]}>{ui.completed ? 'PROLOGUE COMPLETE' : 'المهمة فشلت'}</Text>
            <Text style={styles.modalBody}>{ui.completed ? `تم إنهاء التدريب. الأهداف المحيّدة: ${ui.score}` : 'أعد المحاولة وعدّل طريقة الاشتباك.'}</Text>
            <View style={styles.modalActions}>
              <GameButton label="إعادة" onPress={restart} />
              <GameButton label="القائمة" onPress={onExit} />
            </View>
          </View>
        </View>
      )}
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  root: { flex: 1, backgroundColor: colors.bg },
  topBar: { position: 'absolute', top: 8, left: 18, right: 18, minHeight: 64, flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between', zIndex: 20, gap: 14 },
  missionBlock: { flex: 1.4 },
  missionTitle: { color: colors.text, fontWeight: '900', fontSize: 17 },
  objective: { color: colors.muted, fontSize: 12, marginTop: 3 },
  statusBlock: { width: 145 },
  statusText: { color: colors.text, fontSize: 12, fontWeight: '800' },
  healthTrack: { height: 8, borderRadius: 8, backgroundColor: '#3f1d24', overflow: 'hidden', marginTop: 5 },
  healthFill: { height: '100%', backgroundColor: colors.danger },
  weaponBlock: { minWidth: 150, alignItems: 'flex-end' },
  weaponName: { color: colors.accent, fontSize: 14, fontWeight: '900' },
  ammoText: { color: colors.text, fontSize: 20, fontWeight: '900' },
  smallText: { color: colors.muted, fontSize: 10 },
  arena: { position: 'absolute', backgroundColor: '#141c2b', borderWidth: 2, borderColor: '#334155', overflow: 'hidden' },
  gridLineA: { position: 'absolute', left: '50%', top: 0, bottom: 0, width: 1, backgroundColor: '#263244' },
  gridLineB: { position: 'absolute', top: '50%', left: 0, right: 0, height: 1, backgroundColor: '#263244' },
  cover: { position: 'absolute', backgroundColor: colors.cover, borderWidth: 1, borderColor: '#64748b' },
  player: { position: 'absolute', backgroundColor: colors.player, borderWidth: 3, borderColor: colors.info, zIndex: 10 },
  aimLine: { position: 'absolute', borderRadius: 10, backgroundColor: colors.info, zIndex: 11 },
  enemy: { position: 'absolute', backgroundColor: colors.enemy, borderWidth: 2, borderColor: '#fecaca', overflow: 'hidden', zIndex: 8 },
  markedEnemy: { borderColor: colors.accent, borderWidth: 4 },
  enemyHealth: { position: 'absolute', left: 0, bottom: 0, height: 4, backgroundColor: '#22c55e' },
  bullet: { position: 'absolute', backgroundColor: colors.bullet, zIndex: 12 },
  enemyBullet: { position: 'absolute', backgroundColor: colors.enemyBullet, zIndex: 12 },
  waypoint: { position: 'absolute', alignItems: 'center', justifyContent: 'center', backgroundColor: '#16a34a55', borderWidth: 3, borderColor: colors.success, zIndex: 4 },
  extraction: { position: 'absolute', alignItems: 'center', justifyContent: 'center', backgroundColor: '#16a34a66', borderWidth: 3, borderColor: colors.success, zIndex: 4 },
  zoneText: { color: '#dcfce7', fontSize: 11, fontWeight: '900' },
  leftControls: { position: 'absolute', left: 18, bottom: 18, alignItems: 'center', gap: 5, zIndex: 30 },
  rightControls: { position: 'absolute', right: 18, bottom: 18, flexDirection: 'row', alignItems: 'flex-end', gap: 10, zIndex: 30 },
  quitButton: { position: 'absolute', right: 18, top: 82, zIndex: 31 },
  aimPad: { alignItems: 'center', gap: 5 },
  controlRow: { flexDirection: 'row', gap: 5 },
  actionRow: { flexDirection: 'row', gap: 8, alignItems: 'flex-end' },
  utilityColumn: { gap: 6 },
  fireButton: { width: 82, height: 82, borderRadius: 41, backgroundColor: '#991b1bcc', borderWidth: 2, borderColor: '#fb7185', alignItems: 'center', justifyContent: 'center' },
  firePressed: { transform: [{ scale: 0.95 }], backgroundColor: '#dc2626' },
  fireText: { color: '#fff', fontSize: 18, fontWeight: '900' },
  overlay: { ...StyleSheet.absoluteFillObject, backgroundColor: '#020617cc', alignItems: 'center', justifyContent: 'center', zIndex: 100 },
  modal: { width: '48%', minWidth: 360, backgroundColor: colors.panel, borderRadius: 24, padding: 28, borderWidth: 1, borderColor: '#475569' },
  modalTitle: { color: colors.danger, fontSize: 30, fontWeight: '900', textAlign: 'center' },
  modalBody: { color: colors.muted, fontSize: 15, textAlign: 'center', marginTop: 10 },
  modalActions: { flexDirection: 'row', justifyContent: 'center', gap: 12, marginTop: 22 },
});
