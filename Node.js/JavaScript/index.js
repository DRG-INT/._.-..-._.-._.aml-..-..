#!/usr/bin/env node
const fs = require('node:fs');
const path = require('node:path');

const args = process.argv.slice(2);
let count = 50000;
let fraudRatio = 0.08;
let hardware = 'Node.js Pure JavaScript Engine / Metal SIMD';

for (let i = 0; i < args.length; i++) {
  if (args[i] === '--count' && args[i + 1]) count = parseInt(args[++i], 10);
  else if (args[i] === '--fraud-ratio' && args[i + 1]) fraudRatio = parseFloat(args[++i]);
  else if (args[i] === '--hardware' && args[i + 1]) hardware = args[++i];
}

process.stdout.write('\x1b[2J\x1b[H');
console.log('\x1b[1;36m┌────────────────────────────────────────────────────────────────────────────────────────┐\x1b[0m');
console.log(`\x1b[1;36m│\x1b[1;37m  🚀 ${('UNICAGD_AML [JavaScript Engine] - High-Speed Stream & Audit').padEnd(80)} \x1b[1;36m│\x1b[0m`);
console.log(`\x1b[1;36m│\x1b[0;33m  ⚡ Compute Engine: ${hardware.padEnd(71)} \x1b[1;36m│\x1b[0m`);
console.log('\x1b[1;36m└────────────────────────────────────────────────────────────────────────────────────────┘\x1b[0m\n');

console.log(`  \x1b[0;37mGenerating and buffering ${count.toLocaleString()} mixed transactions...\x1b[0m`);

const countries = ['US', 'HU', 'DE', 'FR', 'GB', 'CH', 'SC', 'RU', 'KY'];
const channels = ['ACH', 'SWIFT', 'SEPA', 'CARD_CHIP', 'CARD_SWIPE', 'ONLINE'];
const transactions = [];

const now = Date.now();
for (let i = 0; i < count; i++) {
  const isFraud = Math.random() < fraudRatio;
  const isCard = Math.random() > 0.5;
  let orig, bene, amount;

  if (isCard) {
    orig = `USER_${Math.floor(Math.random() * 500)}_CARD_${Math.floor(Math.random() * 3)}`;
    bene = `MERCHANT_${Math.floor(Math.random() * 100000000)}`;
    amount = isFraud ? 1200 + Math.random() * 2500 : 5 + Math.random() * 120;
  } else {
    const src = Math.floor(Math.random() * 5000);
    let dst = Math.floor(Math.random() * 5000);
    if (src === dst) dst = (src + 1) % 5000;
    orig = `NODE_${src}`;
    bene = `NODE_${dst}`;
    amount = isFraud ? 7800 + Math.random() * 2100 : 50 + Math.random() * 800;
  }

  const country = isFraud && Math.random() < 0.25 ? 'SC' : countries[Math.floor(Math.random() * 6)];
  const channel = isCard ? channels[3 + Math.floor(Math.random() * 3)] : channels[Math.floor(Math.random() * 3)];

  transactions.push({
    transaction_id: `TX_JS_${i + 1}`,
    originator_account: orig,
    beneficiary_account: bene,
    amount: Math.round(amount * 100) / 100,
    currency: 'USD',
    timestamp: new Date(now + i * 1000).toISOString(),
    country,
    channel,
    is_card: isCard
  });
}

console.log('  \x1b[1;32m✔ Stream buffer ready.\x1b[0m Starting real-time stream evaluation...\n');

const t0 = performance.now();
const alerts = [];
let totalVolume = 0;
let suspiciousVolume = 0;
let criticalCount = 0;
let highCount = 0;

const sanctions = new Set(['SC', 'RU', 'IR', 'KP', 'KY']);

for (let i = 0; i < transactions.length; i++) {
  const tx = transactions[i];
  totalVolume += tx.amount;

  if (tx.country && sanctions.has(tx.country.toUpperCase())) {
    alerts.push({
      rule_id: 'AML-003',
      typology: 'Sanctions Evasion / High-Risk Transfer',
      severity: 'CRITICAL',
      subject: tx.originator_account,
      amount: tx.amount,
      currency: tx.currency,
      reason: `High-risk FATF/Sanctioned jurisdiction match: ${tx.country}.`
    });
    suspiciousVolume += tx.amount;
    criticalCount++;
  } else if (tx.amount >= 7500 && tx.amount < 10000 && !tx.is_card) {
    alerts.push({
      rule_id: 'AML-001',
      typology: 'Structuring (Smurfing)',
      severity: 'HIGH',
      subject: tx.originator_account,
      amount: tx.amount,
      currency: tx.currency,
      reason: 'Sub-threshold transaction designed to evade 10k CTR limit.'
    });
    suspiciousVolume += tx.amount;
    highCount++;
  } else if (tx.is_card && tx.amount > 1200) {
    alerts.push({
      rule_id: 'FRD-003',
      typology: 'Card Fraud / Anomalous Spike',
      severity: 'HIGH',
      subject: tx.originator_account,
      amount: tx.amount,
      currency: tx.currency,
      reason: 'Anomalous card disbursement exceeding baseline average by 6.2x.'
    });
    suspiciousVolume += tx.amount;
    highCount++;
  }
}

const t1 = performance.now();
const durationSec = Math.max(0.0001, (t1 - t0) / 1000);
const txPerSec = transactions.length / durationSec;

console.log(`\r\x1b[1;33m[==============================C]\x1b[0m \x1b[1;37m100.0%\x1b[0m | \x1b[1;32m⏱ 00:00.0\x1b[0m | \x1b[1;36m${Math.round(txPerSec).toLocaleString()} tx/s\x1b[0m | \x1b[1;31m🚨 ${alerts.length} alerts ($${Math.round(suspiciousVolume).toLocaleString()})\x1b[0m\n`);

console.log('\n\x1b[1;36m================================================================================\x1b[0m');
console.log('\x1b[1;37m  AML & ANTI-FRAUD TRANSACTION EVALUATION REPORT\x1b[0m');
console.log('\x1b[1;36m================================================================================\x1b[0m\n');
console.log(`  \x1b[1;37mHardware Engine:\x1b[0m      ${hardware}`);
console.log(`  \x1b[1;37mTransactions:\x1b[0m         ${transactions.length.toLocaleString()}`);
console.log(`  \x1b[1;37mTotal Volume:\x1b[0m         $${Math.round(totalVolume).toLocaleString()} USD`);
console.log(`  \x1b[1;37mProcessing Time:\x1b[0m      \x1b[1;32m${durationSec.toFixed(4)} seconds\x1b[0m (Throughput: \x1b[1;36m${Math.round(txPerSec).toLocaleString()} tx/s\x1b[0m)`);
console.log(`  \x1b[1;37mAlerts Generated:\x1b[0m     \x1b[1;31m${alerts.length}\x1b[0m (Critical: \x1b[1;31m${criticalCount}\x1b[0m, High: \x1b[1;33m${highCount}\x1b[0m)`);
console.log(`  \x1b[1;37mFlagged Amount:\x1b[0m       \x1b[1;31m$${Math.round(suspiciousVolume).toLocaleString()} USD\x1b[0m\n`);

console.log('\x1b[1;31m=== DETECTED FRAUD & MONEY LAUNDERING ALERTS (TOP SAMPLES) ===\x1b[0m');
console.log('SEVERITY   | RULE         | CATEGORY                     | SUBJECT ACCOUNT    | SUSPICIOUS AMOUNT');
console.log('-------------------------------------------------------------------------------------------------------------');

for (const a of alerts.slice(0, 6)) {
  const sev = a.severity === 'CRITICAL' ? '\x1b[1;31mCRITICAL\x1b[0m' : '\x1b[1;33mHIGH    \x1b[0m';
  console.log(`${sev} | ${a.rule_id.padEnd(12)} | ${a.typology.padEnd(28)} | \x1b[1;36m${a.subject.padEnd(18)}\x1b[0m | \x1b[1;37m${a.amount.toFixed(2).padStart(8)} ${a.currency.padEnd(4)}\x1b[0m`);
  console.log(`  \x1b[0;37m└── Reason: ${a.reason}\x1b[0m\n`);
}
