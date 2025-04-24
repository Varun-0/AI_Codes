% filepath: medical_expert_system.pl
% Simple Medical Diagnosis Expert System

% Facts - symptoms that can be observed
symptom(fever).
symptom(cough).
symptom(headache).
symptom(sore_throat).
symptom(runny_nose).
symptom(fatigue).
symptom(body_ache).
symptom(rash).
symptom(nausea).
symptom(shortness_of_breath).
symptom(chest_pain).
symptom(dizziness).

% Knowledge base - patient's symptoms
has_symptom(patient, fever).
has_symptom(patient, cough).
has_symptom(patient, headache).
has_symptom(patient, fatigue).

% Rules for diagnosing conditions (at least 10 rules)
% Rule 1
diagnosis(Patient, common_cold) :-
    has_symptom(Patient, cough),
    has_symptom(Patient, runny_nose),
    has_symptom(Patient, sore_throat),
    \+ has_symptom(Patient, fever).

% Rule 2
diagnosis(Patient, flu) :-
    has_symptom(Patient, fever),
    has_symptom(Patient, cough),
    has_symptom(Patient, body_ache),
    has_symptom(Patient, fatigue).

% Rule 3
diagnosis(Patient, allergies) :-
    has_symptom(Patient, runny_nose),
    has_symptom(Patient, cough),
    \+ has_symptom(Patient, fever),
    \+ has_symptom(Patient, body_ache).

% Rule 4
diagnosis(Patient, strep_throat) :-
    has_symptom(Patient, sore_throat),
    has_symptom(Patient, fever),
    \+ has_symptom(Patient, cough).

% Rule 5
diagnosis(Patient, migraine) :-
    has_symptom(Patient, headache),
    has_symptom(Patient, dizziness),
    has_symptom(Patient, nausea).

% Rule 6
diagnosis(Patient, covid19) :-
    has_symptom(Patient, fever),
    has_symptom(Patient, cough),
    has_symptom(Patient, fatigue),
    has_symptom(Patient, shortness_of_breath).

% Rule 7
diagnosis(Patient, food_poisoning) :-
    has_symptom(Patient, nausea),
    has_symptom(Patient, body_ache),
    has_symptom(Patient, fatigue).

% Rule 8
diagnosis(Patient, pneumonia) :-
    has_symptom(Patient, fever),
    has_symptom(Patient, cough),
    has_symptom(Patient, shortness_of_breath),
    has_symptom(Patient, chest_pain).

% Rule 9
diagnosis(Patient, dehydration) :-
    has_symptom(Patient, dizziness),
    has_symptom(Patient, fatigue),
    has_symptom(Patient, headache).

% Rule 10
diagnosis(Patient, skin_infection) :-
    has_symptom(Patient, rash),
    has_symptom(Patient, fever).

% Rule 11
diagnosis(Patient, stress) :-
    has_symptom(Patient, headache),
    has_symptom(Patient, fatigue),
    \+ has_symptom(Patient, fever),
    \+ has_symptom(Patient, cough).

% Helper predicate to add symptoms interactively
add_symptom(Patient, Symptom) :-
    symptom(Symptom),
    assertz(has_symptom(Patient, Symptom)),
    write('Added symptom: '), write(Symptom), nl.

% Helper predicate to remove symptoms
remove_symptom(Patient, Symptom) :-
    retract(has_symptom(Patient, Symptom)),
    write('Removed symptom: '), write(Symptom), nl.

% Helper predicate to clear all symptoms for a patient
clear_symptoms(Patient) :-
    retractall(has_symptom(Patient, _)),
    write('All symptoms cleared for '), write(Patient), nl.

% Helper predicate to list all possible symptoms
list_symptoms :-
    write('Available symptoms:'), nl,
    symptom(X),
    write('- '), write(X), nl,
    fail.
list_symptoms.

% Helper predicate to list patient's current symptoms
patient_symptoms(Patient) :-
    write('Current symptoms for '), write(Patient), write(':'), nl,
    has_symptom(Patient, X),
    write('- '), write(X), nl,
    fail.
patient_symptoms(_).

% Main interface to diagnose based on current symptoms
diagnose(Patient) :-
    write('Possible diagnoses for '), write(Patient), write(':'), nl,
    diagnosis(Patient, Condition),
    write('- '), write(Condition), nl,
    fail.
diagnose(_) :-
    write('End of diagnoses.'), nl.

% Example of how to use the system:
% ?- list_symptoms.                  % Shows all possible symptoms
% ?- clear_symptoms(patient).        % Clears symptoms for patient
% ?- add_symptom(patient, fever).    % Add symptoms one by one
% ?- add_symptom(patient, cough).
% ?- add_symptom(patient, fatigue).
% ?- patient_symptoms(patient).      % Shows current symptoms
% ?- diagnose(patient).              % Gets possible diagnoses
