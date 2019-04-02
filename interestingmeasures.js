// interstingness measures for association rule mining

function confidence(N_A, N_B, N_AB, N) {
  return N_A / N_AB;
}

function lift(N_A, N_B, N_AB, N) {
  return N_AB * N / N_A / N_B;
}

function improvedLift(N_A, N_B, N_AB, N) {
  return N_AB * (N - N_A) / (N_A * (N_B - N_AB));
}


function kulc(N_A, N_B, N_AB, N) {
  return 0.5 * (N_AB / N_B + N_AB / N_A);
}

function imbalanceRatio(N_A, N_B, N_AB, N) {
  return Math.abs(N_A - N_B) / (N_A + N_B - N_AB);
}

function biConfidence(N_A, N_B, N_AB, N) {
  return (N_AB / N_A) - ((N_B - N_AB) / (N - N_A));
}

function biImprove(N_A, N_B, N_AB, N) {
  // const biImprove = (N_AB / N_A - N_B / N) * N_A / (N - N_A);
  return (N_AB / N - N_A * N_B / N / N) / ((N - N_A) / N);
}

function jaccard(N_A, N_B, N_AB, N) {
  return N_AB / (N_A + N_B - N_AB);
}

function conviction(N_A, N_B, N_AB, N) {
  return (N_A / N * (N - N_B) / N) / ((N_A - N_AB) / N);
}

function sebagSchoenauer(N_A, N_B, N_AB, N) {
  return N_AB / (N_A - N_AB);
}

function certaintyFactor(N_A, N_B, N_AB, N) {
  return (N_AB / N_A - N_B / N) / (1 - N_B / N);
}

function lift(N_A, N_B, N_AB, N) {
  return (N_AB * N) / (N_A * N_B);
}

function improve(N_A, N_B, N_AB, N) {
  return (N_AB / N_A - N_B / N);
}



module.exports = {
  confidence,
  lift,
  improvedLift,
  kulc,
  imbalanceRatio,
  biConfidence,
  biImprove,
  jaccard,
  sebagSchoenauer,
  certaintyFactor
};