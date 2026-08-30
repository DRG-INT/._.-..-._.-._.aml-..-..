import { AmlEngine } from './engine/AmlEngine';
import { TuiRenderer } from './tui/TuiRenderer';

/*
 * CLI entry point for TypeScript AML engine.
 * CLI belépési pont a TypeScript AML motorhoz.
 */
async function main() {
  /* Parse command-line arguments / Parancssori argumentumok feldolgozása */
  const args = process.argv.slice(2);
  /* Default transaction count / Alapértelmezett tranzakciószám */
  let count = 50000;
  /* Default fraud ratio / Alapértelmezett csalási arány */
  let fraudRatio = 0.08;
  /* Hardware description / Hardver leírás */
  let hardware = 'Node.js TypeScript SIMD / Metal GPU';

  for (let i = 0; i < args.length; i++) {
    /* Transaction count parameter / Tranzakciószám paraméter */
    if (args[i] === '--count' && args[i + 1]) count = parseInt(args[++i], 10);
    /* Fraud ratio parameter / Csalási arány paraméter */
    else if (args[i] === '--fraud-ratio' && args[i + 1]) fraudRatio = parseFloat(args[++i]);
    /* Hardware override parameter / Hardver felülírási paraméter */
    else if (args[i] === '--hardware' && args[i + 1]) hardware = args[++i];
  }

  /* Render engine banner / Motor banner megjelenítése */
  TuiRenderer.renderBanner('UNICAGD_AML [TypeScript Engine] - Turbocharged Stream & Audit', hardware);

  /* Initialize AML engine / AML motor inicializálása */
  const engine = new AmlEngine(hardware);
  console.log(`  \x1b[0;37mGenerating and pre-buffering ${count.toLocaleString()} mixed transactions...\x1b[0m`);

  /* Generate synthetic transaction stream / Szintetikus tranzakciófolyam generálása */
  const transactions = engine.generateSyntheticStream(count, fraudRatio);
  console.log('  \x1b[1;32m✔ Stream buffer ready.\x1b[0m Starting real-time stream evaluation...\n');

  /* Preview first 20 transactions / Első 20 tranzakció előnézete */
  const preview = transactions.slice(0, Math.min(20, count));
  for (const tx of preview) {
    /* Sanctions evasion check / Szankciókerülő ellenőrzés */
    const alert = (tx.country === 'SC') ? {
      alert_id: 'ALT_PREVIEW',
      rule_id: 'AML-003',
      rule_name: 'Sanctions Flag',
      category: 'AML_HIGH_RISK_JURISDICTION',
      severity: 'CRITICAL' as const,
      subject_account: tx.originator_account,
      total_suspicious_amount: tx.amount,
      currency: tx.currency,
      confidence_score: 0.98,
      reason: 'Sanctioned jurisdiction match: SC',
      typology: 'Sanctions Evasion',
      timestamp: tx.timestamp
    } : undefined;

    /* Render log entry / Naplóbejegyzés megjelenítése */
    TuiRenderer.renderLogEntry(tx, alert);
  }

  console.log(`\n  \x1b[0;37m... (accelerating remaining ${(count - preview.length).toLocaleString()} transactions) ...\x1b[0m\n`);

  /* Evaluate full transaction batch / Teljes tranzakció köteg értékelése */
  const report = engine.evaluate(transactions);
  TuiRenderer.renderProgress(count, count, report.summary.execution_time_seconds, report.summary.throughput_tx_per_sec, report.summary.alerts_count, report.summary.total_suspicious_amount);
  TuiRenderer.renderFinalReport(report);
}

void main();
