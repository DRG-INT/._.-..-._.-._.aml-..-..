import { EvaluationReport, RiskAlert, Transaction } from '../types';

/*
 * TUI Renderer - TypeScript Terminal UI.
 * TUI Megjelenítő - TypeScript Terminál felület.
 *
 * Handles all console output including banners, progress bars, and reports.
 * Kezeli az összes konzol kimenetet, beleértve a banner-eket, progressz sávokat és jelentéseket.
 */
export class TuiRenderer {
  /*
   * Render application banner.
   * Alkalmazás banner megjelenítése.
   *
   * @param title Banner title / Banner cím.
   * @param hardware Hardware description / Hardver leírás.
   */
  public static renderBanner(title: string, hardware: string): void {
    process.stdout.write('\x1b[2J\x1b[H'); // clear screen / képernyő törlése
    console.log('\x1b[1;36m┌────────────────────────────────────────────────────────────────────────────────────────┐\x1b[0m');
    console.log(`\x1b[1;36m│\x1b[1;37m  🚀 ${title.padEnd(80)} \x1b[1;36m│\x1b[0m`);
    console.log(`\x1b[1;36m│\x1b[0;33m  ⚡ Compute Engine: ${hardware.padEnd(71)} \x1b[1;36m│\x1b[0m`);
    console.log('\x1b[1;36m└────────────────────────────────────────────────────────────────────────────────────────┘\x1b[0m\n');
  }

  /*
   * Render progress bar with animation.
   * Animált progressz sáv megjelenítése.
   *
   * @param current Current progress value / Jelenlegi folyamat érték.
   * @param total Total items / Összes elem.
   * @param elapsedSec Elapsed seconds / Eltelt másodperc.
   * @param txPerSec Throughput in transactions per second / Tranzakciók per másodperc.
   * @param alertsCount Number of alerts / Riasztások száma.
   * @param flaggedVolume Flagged amount / Flagelt összeg.
   */
  public static renderProgress(current: number, total: number, elapsedSec: number, txPerSec: number, alertsCount: number, flaggedVolume: number): void {
    const barWidth = 30;
    const progress = total > 0 ? Math.min(1.0, current / total) : 1.0;
    const filled = Math.floor(progress * barWidth);

    /* Pacman animation bar / Pacman animációs sáv */
    let pacmanBar = '';
    for (let i = 0; i < barWidth; i++) {
      if (i < filled - 1) pacmanBar += '=';
      else if (i === filled - 1) pacmanBar += (current % 2 === 0) ? 'C' : 'c'; // Pacman mouth / Pacman száj
      else pacmanBar += (i % 3 === 0) ? 'o' : '-'; // Pac-dots / Pac-dots
    }

    const mins = Math.floor(elapsedSec / 60);
    const secs = (elapsedSec % 60).toFixed(1);

    /* Pinned dashboard line / Rögzített dashboard sor */
    process.stdout.write(
      `\r\x1b[1;33m[${pacmanBar}]\x1b[0m \x1b[1;37m${(progress * 100).toFixed(1)}%\x1b[0m | \x1b[1;32m⏱ ${String(mins).padStart(2, '0')}:${String(secs).padStart(4, '0')}\x1b[0m | \x1b[1;36m${Math.round(txPerSec).toLocaleString()} tx/s\x1b[0m | \x1b[1;31m🚨 ${alertsCount} alerts ($${Math.round(flaggedVolume).toLocaleString()})\x1b[0m\n`
    );
  }

  /*
   * Render a single transaction log entry.
   * Egyetlen tranzakció naplóbejegyzés megjelenítése.
   *
   * @param tx Transaction data / Tranzakció adatok.
   * @param alert Optional alert data / Opcionális riasztás adatok.
   */
  public static renderLogEntry(tx: Transaction, alert?: RiskAlert): void {
    if (alert) {
      /* Minimal bold red/yellow highlight ONLY on suspicious alerts / Minimális félkövér piros/sárga kiemelés CSAK gyanús riasztásoknál */
      console.log(
        `  \x1b[1;31m▶ [${alert.severity} ALERT ${alert.rule_id}]\x1b[0m \x1b[1;37m${tx.originator_account.padEnd(16)}\x1b[0m ➔ \x1b[1;37m${tx.beneficiary_account.padEnd(16)}\x1b[0m | \x1b[1;31m${tx.amount.toFixed(2).padStart(8)} ${tx.currency}\x1b[0m | \x1b[0;33m${alert.reason}\x1b[0m`
      );
    } else {
      /* Muted normal transaction log stream / Halkított normál tranzakció naplófolyam */
      console.log(
        `  \x1b[0;37m• [STREAM]\x1b[0m ${tx.originator_account.padEnd(18)} ➔ ${tx.beneficiary_account.padEnd(18)} | ${tx.amount.toFixed(2).padStart(8)} ${tx.currency.padEnd(4)} | ${tx.channel || 'CORE'}`
      );
    }
  }

  /*
   * Render final evaluation report.
   * Végső értékelési jelentés megjelenítése.
   *
   * @param report Evaluation report data / Értékelési jelentés adatok.
   */
  public static renderFinalReport(report: EvaluationReport): void {
    console.log('\n\x1b[1;36m================================================================================\x1b[0m');
    console.log('\x1b[1;37m  AML & ANTI-FRAUD TRANSACTION EVALUATION REPORT\x1b[0m');
    console.log('\x1b[1;36m================================================================================\x1b[0m\n');
    console.log(`  \x1b[1;37mHardware Engine:\x1b[0m      ${report.summary.hardware}`);
    console.log(`  \x1b[1;37mTransactions:\x1b[0m         ${report.summary.total_transactions_analyzed.toLocaleString()}`);
    console.log(`  \x1b[1;37mTotal Volume:\x1b[0m         $${report.summary.total_volume_analyzed.toLocaleString()} USD`);
    console.log(
      `  \x1b[1;37mProcessing Time:\x1b[0m      \x1b[1;32m${report.summary.execution_time_seconds.toFixed(4)} seconds\x1b[0m (Throughput: \x1b[1;36m${Math.round(report.summary.throughput_tx_per_sec).toLocaleString()} tx/s\x1b[0m)`
    );
    console.log(
      `  \x1b[1;37mAlerts Generated:\x1b[0m     \x1b[1;31m${report.summary.alerts_count}\x1b[0m (Critical: \x1b[1;31m${report.summary.critical_alerts}\x1b[0m, High: \x1b[1;33m${report.summary.high_alerts}\x1b[0m, Med: ${report.summary.medium_alerts})`
    );
    console.log(`  \x1b[1;37mFlagged Amount:\x1b[0m       \x1b[1;31m$${report.summary.total_suspicious_amount.toLocaleString()} USD\x1b[0m\n`);

    console.log('\x1b[1;31m=== DETECTED FRAUD & MONEY LAUNDERING ALERTS (TOP SAMPLES) ===\x1b[0m');
    console.log('SEVERITY   | RULE         | CATEGORY                     | SUBJECT ACCOUNT    | SUSPICIOUS AMOUNT');
    console.log('-------------------------------------------------------------------------------------------------------------');

    const top = report.alerts.slice(0, 6);
    for (const a of top) {
      const sev = a.severity === 'CRITICAL' ? '\x1b[1;31mCRITICAL\x1b[0m' : '\x1b[1;33mHIGH    \x1b[0m';
      console.log(`${sev} | ${a.rule_id.padEnd(12)} | ${a.typology.padEnd(28)} | \x1b[1;36m${a.subject_account.padEnd(18)}\x1b[0m | \x1b[1;37m${a.total_suspicious_amount.toFixed(2).padStart(8)} ${a.currency.padEnd(4)}\x1b[0m`);
      console.log(`  \x1b[0;37m└── Reason: ${a.reason}\x1b[0m\n`);
    }
  }
}
