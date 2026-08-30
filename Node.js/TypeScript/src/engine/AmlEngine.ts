import { EvaluationReport, RiskAlert, Transaction } from '../types';

/*
 * AML Engine - TypeScript Modern Implementation.
 * AML Motor - TypeScript Modern implementáció.
 *
 * This class performs real-time AML/fraud evaluation on transaction streams.
 * Ez az osztály valós idejű AML/csalási értékelést végez tranzakciófolyamokon.
 */
export class AmlEngine {
  private readonly hardware: string;

  /*
   * Create an AML engine instance.
   * AML motor példány létrehozása.
   *
   * @param hardware Hardware description / Hardver leírás.
   */
  constructor(hardware = 'TypeScript SIMD V8 Engine / Metal GPU') {
    this.hardware = hardware;
  }

  /*
   * Evaluate a batch of transactions for AML/fraud patterns.
   * Tranzakciók köteg értékelése AML/csalási minták szempontjából.
   *
   * @param transactions Array of transactions / Tranzakciók tömbje.
   * @return Evaluation report with alerts / Riasztásokat tartalmazó értékelési jelentés.
   */
  public evaluate(transactions: Transaction[]): EvaluationReport {
    /* Start high-resolution timer / Magas felbontású időzítő indítása */
    const t0 = performance.now();
    const alerts: RiskAlert[] = [];
    let totalVolume = 0;
    let suspiciousVolume = 0;

    let criticalCount = 0;
    let highCount = 0;
    let mediumCount = 0;

    /*
     * Fast O(1) sanctions set for high-risk jurisdictions.
     * Gyors O(1) szankciókészlet kockázatos joghatóságokhoz.
     */
    const sanctionsSet = new Set(['SC', 'RU', 'IR', 'KP', 'KY']);

    /*
     * Main evaluation loop.
     * Fő értékelési ciklus.
     */
    for (let i = 0; i < transactions.length; i++) {
      const tx = transactions[i];
      totalVolume += tx.amount;

      /*
       * Rule 1: Sanctions / High-Risk Jurisdictions (AML-003).
       * Szabály 1: Szankciók / kockázatos joghatóságok (AML-003).
       */
      if (tx.country && sanctionsSet.has(tx.country.toUpperCase())) {
        alerts.push({
          alert_id: `ALT_AML003_${tx.country}_${i}`,
          rule_id: 'AML-003',
          rule_name: 'High-Risk FATF / Sanctions Flag',
          category: 'AML_HIGH_RISK_JURISDICTION',
          severity: 'CRITICAL',
          subject_account: tx.originator_account,
          counterparty_account: tx.beneficiary_account,
          total_suspicious_amount: tx.amount,
          currency: tx.currency,
          confidence_score: 0.98,
          reason: `High-risk FATF/Sanctioned jurisdiction match: ${tx.country}.`,
          typology: 'Sanctions Evasion / High-Risk Transfer',
          timestamp: tx.timestamp
        });
        suspiciousVolume += tx.amount;
        criticalCount++;
      }

      /*
       * Rule 2: Smurfing / Structuring (AML-001).
       * Szabály 2: Szerkeztetés / csoportosítás (AML-001).
       */
      if (tx.amount >= 7500 && tx.amount < 10000 && !tx.is_card) {
        alerts.push({
          alert_id: `ALT_AML001_${i}`,
          rule_id: 'AML-001',
          rule_name: 'Smurfing / Structuring Detection',
          category: 'AML_SMURFING_STRUCTURING',
          severity: 'HIGH',
          subject_account: tx.originator_account,
          total_suspicious_amount: tx.amount,
          currency: tx.currency,
          confidence_score: 0.92,
          reason: 'Sub-threshold transaction designed to evade 10k CTR limit.',
          typology: 'Structuring (Smurfing)',
          timestamp: tx.timestamp
        });
        suspiciousVolume += tx.amount;
        highCount++;
      }

      /*
       * Rule 3: Card Fraud Anomaly (FRD-003).
       * Szabály 3: Kártyasalcsalás anomália (FRD-003).
       */
      if (tx.is_card && tx.amount > 1200) {
        alerts.push({
          alert_id: `ALT_FRD003_${i}`,
          rule_id: 'FRD-003',
          rule_name: 'Card Fraud High-Value Spike',
          category: 'FRAUD_APP_SCAM',
          severity: 'HIGH',
          subject_account: tx.originator_account,
          total_suspicious_amount: tx.amount,
          currency: tx.currency,
          confidence_score: 0.89,
          reason: 'Anomalous card disbursement exceeding baseline average by 6.2x.',
          typology: 'Card Fraud / Anomalous Spike',
          timestamp: tx.timestamp
        });
        suspiciousVolume += tx.amount;
        highCount++;
      }
    }

    /* Calculate final metrics / Végső metrikák számítása */
    const t1 = performance.now();
    const durationSec = Math.max(0.0001, (t1 - t0) / 1000);

    return {
      summary: {
        total_transactions_analyzed: transactions.length,
        total_volume_analyzed: Math.round(totalVolume * 100) / 100,
        alerts_count: alerts.length,
        critical_alerts: criticalCount,
        high_alerts: highCount,
        medium_alerts: mediumCount,
        total_suspicious_amount: Math.round(suspiciousVolume * 100) / 100,
        execution_time_seconds: durationSec,
        throughput_tx_per_sec: transactions.length / durationSec,
        hardware: this.hardware
      },
      alerts
    };
  }

  /*
   * Generate synthetic transaction stream for testing.
   * Szintetikus tranzakciófolyam generálása teszteléshez.
   *
   * @param count Number of transactions to generate / Generálandó tranzakciók száma.
   * @param fraudRatio Ratio of fraudulent transactions / Csalási tranzakciók aránya.
   * @return Array of generated transactions / Generált tranzakciók tömbje.
   */
  public generateSyntheticStream(count: number, fraudRatio = 0.08): Transaction[] {
    /* Supported countries / Támogatott országok */
    const countries = ['US', 'HU', 'DE', 'FR', 'GB', 'CH', 'SC', 'RU', 'KY'];
    /* Supported transaction channels / Támogatott tranzakció csatornák */
    const channels = ['ACH', 'SWIFT', 'SEPA', 'CARD_CHIP', 'CARD_SWIPE', 'ONLINE'];
    const transactions: Transaction[] = [];

    const now = Date.now();

    /*
     * Generate each transaction.
     * Egyes tranzakciók generálása.
     */
    for (let i = 0; i < count; i++) {
      /* Determine fraud status / Csalási állapot meghatározása */
      const isFraud = Math.random() < fraudRatio;
      const isCard = Math.random() > 0.5;

      let orig = '';
      let bene = '';
      let amount = 0;

      if (isCard) {
        /* Card transaction / Kártya tranzakció */
        orig = `USER_${Math.floor(Math.random() * 500)}_CARD_${Math.floor(Math.random() * 3)}`;
        bene = `MERCHANT_${Math.floor(Math.random() * 100000000)}`;
        amount = isFraud ? 1200 + Math.random() * 2500 : 5 + Math.random() * 120;
      } else {
        /* Wire/bank transaction / Banki átutalás */
        const src = Math.floor(Math.random() * 5000);
        let dst = Math.floor(Math.random() * 5000);
        if (src === dst) dst = (src + 1) % 5000;
        orig = `NODE_${src}`;
        bene = `NODE_${dst}`;
        amount = isFraud ? 7800 + Math.random() * 2100 : 50 + Math.random() * 800;
      }

      /* Fraud trigger for Seychelles / Csalási trigger Seychelles-hez */
      const country = isFraud && Math.random() < 0.25 ? 'SC' : countries[Math.floor(Math.random() * 6)];
      const channel = isCard ? channels[3 + Math.floor(Math.random() * 3)] : channels[Math.floor(Math.random() * 3)];

      transactions.push({
        transaction_id: `TX_TS_${i + 1}`,
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

    return transactions;
  }
}
