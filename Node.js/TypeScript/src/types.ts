/*
 * Type definitions for UNICAGD_AML TypeScript engine.
 * Típus definíciók a UNICAGD_AML TypeScript motorhoz.
 */

/*
 * Risk severity levels.
 * Kockázati súlyossági szintek.
 */
export type RiskSeverity = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';

/*
 * Transaction data structure.
 * Tranzakció adatstruktúra.
 *
 * Represents a single financial transaction in the system.
 * Egyetlen pénzügyi tranzakciót képvisel a rendszerben.
 */
export interface Transaction {
  transaction_id: string;        /* Unique transaction ID / Egyedi tranzakció azonosító */
  originator_account: string;    /* Sender account / Küldő számla */
  beneficiary_account: string;   /* Receiver account / Címzett számla */
  amount: number;                /* Transaction amount / Tranzakció összege */
  currency: string;              /* Currency code (USD, EUR, HUF) / Pénznem kód */
  timestamp: string;             /* ISO timestamp / ISO időbélyeg */
  ip_address?: string;           /* Source IP address / Forrás IP cím */
  device_id?: string;            /* Device identifier / Eszköz azonosító */
  country?: string;              /* Country code / Ország kód */
  channel?: string;              /* Transaction channel / Tranzakció csatorna */
  is_card?: boolean;             /* Is card transaction / Kártya tranzakció-e */
}

/*
 * Risk alert data structure.
 * Kockázati riasztás adatstruktúra.
 *
 * Represents a detected AML or fraud pattern.
 * Egy észlelt AML vagy csalási mintát képvisel.
 */
export interface RiskAlert {
  alert_id: string;                    /* Unique alert ID / Egyedi riasztás azonosító */
  rule_id: string;                     /* Rule identifier / Szabály azonosító */
  rule_name: string;                   /* Human-readable rule name / Emberként olvasható szabály név */
  category: string;                    /* Alert category / Riasztás kategória */
  severity: RiskSeverity;              /* Alert severity / Riasztás súlyossága */
  subject_account: string;             /* Subject account number / Tárgy számlaszám */
  counterparty_account?: string;       /* Counterparty account / Ellenfél számla */
  total_suspicious_amount: number;     /* Suspicious amount / Gyanús összeg */
  currency: string;                    /* Currency code / Pénznem kód */
  confidence_score: number;            /* Confidence score (0-1) / Megbízhatósági pontszám */
  reason: string;                      /* Alert reason description / Riasztás indoklása */
  typology: string;                    /* Crime typology / Bűncselekmény tipológia */
  timestamp: string;                   /* Transaction timestamp / Tranzakció időbélyege */
}

/*
 * Evaluation report summary.
 * Értékelési jelentés összefoglaló.
 */
export interface EvaluationReport {
  summary: {
    total_transactions_analyzed: number;   /* Total transactions processed / Feldolgozott tranzakciók összege */
    total_volume_analyzed: number;         /* Total volume in USD / Teljes forgalom USD-ben */
    alerts_count: number;                  /* Total alerts generated / Generált riasztások száma */
    critical_alerts: number;               /* Critical severity alerts / Kritikus súlyosságú riasztások */
    high_alerts: number;                   /* High severity alerts / Magas súlyosságú riasztások */
    medium_alerts: number;                 /* Medium severity alerts / Közép súlyosságú riasztások */
    total_suspicious_amount: number;       /* Total suspicious amount / Gyanús összegek összege */
    execution_time_seconds: number;        /* Processing time in seconds / Feldolgozási idő másodpercben */
    throughput_tx_per_sec: number;         /* Throughput in tx/s / Áteresztőképesség tx/s-ban */
    hardware: string;                      /* Hardware used / Használt hardver */
  };
  alerts: RiskAlert[];                    /* Array of generated alerts / Generált riasztások tömbje */
}
